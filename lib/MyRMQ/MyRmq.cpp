#include "MyRmq.h"



MyRmq::MyRmq(std::string user,std::string pwd,std::string ip,int port)
{
	socket = NULL;
	conn = amqp_new_connection();
	socket = amqp_tcp_socket_new(conn);

	if (!socket)
	{
		die("creating tcp socket fail\n");
	}

	if (amqp_socket_open(socket,ip.c_str(),port))
	{
		die("opening tcp socket fail\n");
	}

	die_on_amqp_error(
		amqp_login(
			conn,
			"/",
			0,
			131072,
			0,
			AMQP_SASL_METHOD_PLAIN,
			user.c_str(),
			pwd.c_str()
		),
		"login"
	);

	amqp_channel_open(conn,1);
	die_on_amqp_error(amqp_get_rpc_reply(conn),"opening channel");
}

MyRmq::~MyRmq()
{

}

void MyRmq::receive(std::string exchange,std::string binding_key,std::string queue_name,Callback fun)
{
	amqp_bytes_t queue_name_bytes = amqp_cstring_bytes(queue_name.c_str());
	amqp_queue_declare_ok_t * r = amqp_queue_declare(
		conn,
		1,
		queue_name_bytes,
		0,
		0,
		0,
		1,
		amqp_empty_table
	);
	die_on_amqp_error(amqp_get_rpc_reply(conn),"declaring queue");

	amqp_queue_bind(
		conn,
		1,
		queue_name_bytes,
		amqp_cstring_bytes(exchange.c_str()),
		amqp_cstring_bytes(binding_key.c_str()),
		amqp_empty_table
	);
	die_on_amqp_error(amqp_get_rpc_reply(conn),"binding queue");

	amqp_basic_consume(
		conn,
		1,
		queue_name_bytes,
		amqp_empty_bytes,
		0,
		1,
		0,
		amqp_empty_table
	);
	die_on_amqp_error(amqp_get_rpc_reply(conn),"consuming");

	for (;;)
	{
		amqp_rpc_reply_t res;
		amqp_envelope_t envelope;
		amqp_maybe_release_buffers(conn);
		res = amqp_consume_message(conn,&envelope,NULL,0);
        if (AMQP_RESPONSE_NORMAL != res.reply_type)
        {
            std::cout << "res.library_error: " << res.library_error << std::endl;
            break;
        }

        char * request = (char*)envelope.message.body.bytes;
        *(request + envelope.message.body.len) = '\0';
        char * response = (*fun)(request);
        std::cout << "request: " << request << std::endl;
        
        if (response != NULL)
        {
        	std::cout << "response: " << response << std::endl;
    		amqp_basic_properties_t props;
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
            props.content_type = amqp_cstring_bytes("application/json");
            props.delivery_mode = AMQP_DELIVERY_PERSISTENT; /* persistent delivery mode */

    		int res_code = amqp_basic_publish(
    			conn,
    			1,
    			amqp_cstring_bytes(""),
    			envelope.message.properties.reply_to,
    			0,
    			0,
    			&props,
    			amqp_cstring_bytes(response)
    		);
            if (AMQP_STATUS_OK != res_code)
            {
                std::cout << "response failed, code: " << res_code << std::endl;
            }
            free(response);
        }

        amqp_destroy_envelope(&envelope);
	}

	die_on_amqp_error(amqp_channel_close(conn,1,AMQP_REPLY_SUCCESS),"closing channel");
	die_on_amqp_error(amqp_connection_close(conn,AMQP_REPLY_SUCCESS),"closing connection");
	die_on_error(amqp_destroy_connection(conn),"ending connection");
}


char * MyRmq::send(std::string exchange,std::string routing_key,std::string data)
{
	amqp_bytes_t reply_to_queue;

	amqp_queue_declare_ok_t * r = amqp_queue_declare(
		conn,
		1,
		amqp_empty_bytes,
		0,
		0,
		0,
		1,
		amqp_empty_table
	);
	die_on_amqp_error(amqp_get_rpc_reply(conn),"declaring queue");

	reply_to_queue = amqp_bytes_malloc_dup(r->queue);
	if (reply_to_queue.bytes == NULL)
	{
		std::cout << "out of memory\n";
		exit(1);
	}

	amqp_basic_properties_t props;
	props._flags = 	AMQP_BASIC_CONTENT_TYPE_FLAG  |
					AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_REPLY_TO_FLAG |
					AMQP_BASIC_CORRELATION_ID_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2;
	props.reply_to = amqp_bytes_malloc_dup(reply_to_queue);
	if (props.reply_to.bytes == NULL)
	{
		std::cout << "out of memory\n";
		exit(1);
	}

	std::cout << "publishing data:" << data.c_str() << std::endl;
	die_on_error(
		amqp_basic_publish(
			conn,
			1,
			amqp_cstring_bytes(exchange.c_str()),
			amqp_cstring_bytes(routing_key.c_str()),
			0,
			0,
			&props,
			amqp_cstring_bytes(data.c_str())
		),
		"publishing"
	);
	amqp_bytes_free(props.reply_to);

	amqp_basic_consume(
		conn,
		1,
		reply_to_queue,
		amqp_empty_bytes,
		0,
		1,
		0,
		amqp_empty_table
	);
	die_on_amqp_error(amqp_get_rpc_reply(conn),"consuming");
	amqp_bytes_free(reply_to_queue);

	amqp_frame_t frame;
	int result;

	amqp_basic_deliver_t *d;
	amqp_basic_properties_t *p;
	size_t body_target;
	size_t body_received;

	char * response_str = NULL;

	for (;;)
	{
		amqp_maybe_release_buffers(conn);
		result = amqp_simple_wait_frame(conn,&frame);
		std::cout << "result: " << result << std::endl;
		if (result < 0)
			break;

		std::cout << "frame type: " << frame.frame_type << std::endl;
		std::cout << "channel: " << frame.channel << std::endl;
		if (frame.frame_type != AMQP_FRAME_METHOD)
			continue;

		std::cout << "method: " << amqp_method_name(frame.payload.method.id) << std::endl;
		if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD)
			continue;

		d = (amqp_basic_deliver_t *)frame.payload.method.decoded;
		std::cout << "delivery: " << (unsigned)d->delivery_tag 
		<< ", exchanges: " << (char*)d->exchange.bytes << 
		", routingkey: " << (char*)d->routing_key.bytes << std::endl; 

		result = amqp_simple_wait_frame(conn,&frame);
		if (result < 0)
			break;

		if (frame.frame_type != AMQP_FRAME_HEADER) {
			std::cout << "expected header" << std::endl;
			abort();
		}

		p = (amqp_basic_properties_t *)frame.payload.properties.decoded;
		if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG){
			std::cout << "content-type: " << (char *)p->content_type.bytes << std::endl;
		}

		std::cout << "-----" << std::endl;

		body_target = (size_t)frame.payload.properties.body_size;
		body_received = 0;

		std::string response = "";
		while( body_received < body_target) {
			result = amqp_simple_wait_frame(conn,&frame);
			if (result < 0)
				break;
			if (frame.frame_type != AMQP_FRAME_BODY){
				std::cout << "expected body\n";
				abort(); 
			}
			body_received += frame.payload.body_fragment.len;
			assert(body_received <= body_target);

			amqp_dump(frame.payload.body_fragment.bytes,frame.payload.body_fragment.len);

			response += std::string((char*)frame.payload.body_fragment.bytes);
		}

		response_str = (char*)malloc(response.size() + 1);
		strcpy(response_str, response.c_str());

		if (body_received != body_target)
			break;
		break;
	}

	die_on_amqp_error(amqp_channel_close(conn,1,AMQP_REPLY_SUCCESS),"closing channel");
	die_on_amqp_error(amqp_connection_close(conn,AMQP_REPLY_SUCCESS),"closing connection");
	die_on_error(amqp_destroy_connection(conn),"ending connection");

	return response_str;
}










