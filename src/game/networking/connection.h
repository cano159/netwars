#ifndef AW_NETWORK_CONNECTION_H
#define AW_NETWORK_CONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <string>
#include <deque>

using boost::asio::ip::tcp;

namespace aw {
  class connection {
  public:
	typedef boost::shared_ptr<connection> ptr;

	virtual ~connection() {}

	static ptr create(boost::asio::io_service& io_service);

	virtual void send_message(const std::string& message);
	void connect(const std::string& host, const std::string& port);
	void start();

	tcp::socket& socket();
	virtual bool has_line();
	virtual std::string receive_line();

  protected:
	connection(boost::asio::io_service& io_service);

	// on_line_received will be called when a line was received.
	// The default implementation pops the received line from the line stack
	// Any subclasses should do the same.
	virtual void on_line_received(const std::string& line);
	virtual void on_connection_closed(const std::string& reason);

	boost::asio::io_service& io_service_;

  private:
	void do_write(std::string message);

	void handle_connect(const boost::system::error_code& error,
						tcp::resolver::iterator endpoint_iterator);
	void handle_write(const boost::system::error_code& error);
	void handle_read(const boost::system::error_code& error);


	tcp::socket socket_;

	boost::asio::streambuf buffer_;

	std::deque<std::string> send_queue_;
	std::deque<std::string> receive_queue_;
  };
}

#endif
