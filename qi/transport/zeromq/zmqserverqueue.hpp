/*
** Author(s):
**  - Cedric GESTES      <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2010 Aldebaran Robotics
*/

#ifndef QI_MESSAGING_TRANSPORT_ZEROMQ_SERVERQUEUE_HPP_
#define QI_MESSAGING_TRANSPORT_ZEROMQ_SERVERQUEUE_HPP_

#include <zmq.hpp>
#include <qi/transport/server.hpp>
#include <qi/transport/common/handlers_pool.hpp>
#include <qi/transport/zeromq/zmqserver.hpp>
#include <string>
#include <boost/thread/mutex.hpp>

namespace qi {
  namespace transport {

  /// <summary>
  /// The server class. It listen for incoming connection from client
  /// and push handlers for those connection to the tread pool.
  /// This class need to be instantiated and run at the beginning of the process.
  /// </summary>
  class ResultHandler;
  class ZMQServerQueue : public Server, public Detail::IServerResponseHandler {
  public:
    /// <summary>The Server class constructor.</summary>
    /// <param name="server_name">
    /// The name given to the server, id for clients to connect.
    /// </param>
    ZMQServerQueue(const std::string & server_name);

    /// <summary>The Server class destructor.
    virtual ~ZMQServerQueue();

    /// <summary>Run the server thread.</summary>
    virtual void run();

    /// <summary>Wait for the server thread to complete its task.</summary>
    void wait();

    /// <summary>Force the server to stop and wait for complete stop.</summary>
    void stop();

    void serverResponseHandler(const std::string &result, void *data = 0);

    ResultHandler *getResultHandler() { return 0; }

    friend void *worker_routine(void *arg);

  private:
    bool                server_running;
    std::string         server_path;
    zmq::context_t      zctx;
    zmq::socket_t       zsocketworkers;
    zmq::socket_t       zsocket;
    boost::mutex        socketMutex;
    HandlersPool        handlersPool;
  };

}
}

#endif /* !QI_MESSAGING_TRANSPORT_ZEROMQ_SERVERQUEUE_HPP_ */
