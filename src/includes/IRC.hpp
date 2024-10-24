#ifndef IRC_H
#define IRC_H
#include <ft_irc.hpp>

template <typename T> class IRC {
public:
  IRC(void){};

  IRC(const ClientArgs args) {
    this->clientArgs.password = args.password;
    this->clientArgs.port = args.port;
  }

  IRC(const IRC &value) : clients(value.clients), channels(value.channels){};
  IRC &operator=(const IRC &value) {
    if (this != &value) {
      clients = value.clients;
      channels = value.channels;
    }
    return *this;
  };
  ~IRC(void) {
    typename std::map<int, Client<T> *>::iterator it = clients.begin();

    for (; it != clients.end(); it++) {
      delete it->second;
    }

    clients.clear();

    typename std::map<std::string, Channel<T> *>::iterator channelIt =
        channels.begin();

    for (; channelIt != channels.end(); channelIt++) {
      delete channelIt->second;
    }

    channels.clear();
  };

  void addClient(Client<T> *client) {
    clients.insert(std::make_pair(client->socket.getFd(), client));
  };

  void addChannel(Channel<T> *channel) {
    channels.insert(std::make_pair(channel->getChannelName(), channel));
  };

  Client<T> *getClient(std::string nickname) {
    typename std::map<int, Client<T> *>::iterator it = clients.begin();

    for (; it != clients.end(); it++) {
      if (it->second->user.nickname == nickname) {
        return it->second;
      }
    }
    return NULL;
  }

  Client<T> *getClient(int fd) {
    typename std::map<int, Client<T> *>::iterator it = clients.find(fd);
    if (it != clients.end())
      return (it->second);
    return (NULL);
  }

  Channel<T> *getChannel(std::string name) {
    try {
      return channels.at(name);
    } catch (const std::exception &error) {
      return NULL;
    }
  }

  void deleteChannel(Channel<T> *channel) {
    try {
      Channel<T> *found = channels.at(channel->getChannelName());
      channels.erase(channel->getChannelName());
      delete found;
    } catch (const std::exception &error) {
    }
  }

  void disconnectClient(Client<T> &client) {
    for (typename std::map<std::string, Channel<T> *>::iterator it =
             channels.begin();
         it != channels.end();) {
      Channel<T> *channel = it->second;
      channel->disconnectClient(client);
      if (channel->getClients().size() == 0) {
        it++;
        deleteChannel(channel);
      } else {
        it++;
      }
    }
    typename std::map<int, Client<T> *>::iterator clientIt =
        clients.find(client.socket.getFd());
    if (clientIt == clients.end())
      return;
    client.socket.erase();
    delete clientIt->second;
    clients.erase(clientIt);
  }

  void broadcast(std::string message) {
    for (typename std::map<int, Client<T> *>::iterator it =
            clients.begin();
        it != clients.end(); it++) {
      it->second->socket.write(message);
    }
  }

  void broadcastToConnectedChannels(Client<T> &from, std::string message) {
    for (typename std::map<std::string, Channel<T> *>::iterator it =
             channels.begin();
         it != channels.end(); it++) {
      if (it->second->isClientInChannel(&from)) {
        it->second->broadcast(from, message);
      }
    }
  }

  bool isRegistered(Client<T> *client) {
    if (!client) return false;
    if (!client->socket._logged) return false;
    return client->user.isComplete();
  }

  std::map<int, Client<T> *> clients;
  std::map<std::string, Channel<T> *> channels;
  ClientArgs clientArgs;
};
#endif
