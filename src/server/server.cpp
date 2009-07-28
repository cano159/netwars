#include "server.h"

#include "json/json_spirit.h"

namespace json = json_spirit;


//utilities
const json::mValue& find_value(const json::mObject& obj, 
							   const std::string& name) {
  json::mObject::const_iterator i = obj.find(name);

  //assert(i != obj.end());
 

  if(i == obj.end())
	return json::mValue::null;

  assert(i->first == name);

  return i->second;
}

server::server(asio::io_service& io)
  : io_service_(io), acceptor_(io, tcp::endpoint(tcp::v4(), 4242)) {

}

void server::start_accept() {
  client_connection::ptr new_connection =
	client_connection::create(io_service_);

  acceptor_.async_accept(new_connection->socket(),
						 boost::bind(&server::handle_accept, 
									 this, 
									 new_connection,
									 boost::asio::placeholders::error));
}

void server::deliver_to_all(const std::string& message) {
  std::clog << "Delivering message to all clients" << std::endl;
  BOOST_FOREACH(const client_connection::ptr& c, conections_) {
	c->send_message(message);
  }
}

void server::deliver_to(client_connection::ptr& to, const std::string& message) {
  to->send_message(message);
}

void server::handle_message(const std::string& message, 
							const client_connection::ptr& from) {
  try {
	json::mValue js;
	json::read_or_throw(message, js);

	json::mValue destination = find_value(js.get_obj(), "destination");

	if(destination == json::mValue::null) 
	  throw std::runtime_error("Didn't find key");

	std::string dest = destination.get_str();

	if(dest == "server") {
	  //Handle it for the server
	} else if(dest == "client") {
	  //Just pass it to all other clients

	  BOOST_FOREACH(const client_connection::ptr& c, conections_) {
		if(c != from)
		  c->send_message(message);
	  }
	}
  } catch(const json::Error_position& e) {
	std::clog << "Got invalid json: " << message << std::endl;
  } catch(const std::exception& e) {
	std::clog << "Got invalid json: " << message << std::endl;
  } catch(...) {
	std::abort();
  }
}

void server::handle_accept(client_connection::ptr new_connection,
						   const boost::system::error_code& error) {
  if (!error) {
	std::clog << "Got a new client" << std::endl;
	conections_.push_back(new_connection);
	new_connection->deliver_callback().connect(boost::bind(&server::handle_message,this,_1,_2));
	new_connection->start();
	start_accept();
  }
}
