#include "replies.hpp"
#include <ft_irc.hpp>

std::string USER(std::vector<std::string> args,
                 Socket<sockaddr_in> &from_socket, IRC<sockaddr_in> &irc) {
  Client<sockaddr_in> *client = irc.getClient(from_socket.getFd());

  if (!client || !from_socket._logged) {
    return ERR_NOTREGISTERED;
  }

  if (args.size() != 4) {
    return ERR_NEEDMOREPARAMS("User", "USER");
  }

  std::string username = args[0];
  std::string realname = args[3];

  if (client->user.isComplete()) {
    return ERR_ALREADYREGISTERED(client->user.nickname);
  }

  client->user.username = username;
  client->user.realname = realname;

  std::string reply;
  reply
      .append(RPL_WELCOME(client->user.nickname,
                          client->user.nickname,
                          client->user.username))
      .append(RPL_YOURHOST(client->user.nickname, SERVER_NAME, "1"))
      .append(RPL_CREATED(client->user.nickname))
      .append(RPL_MYINFO(client->user.nickname));

  return reply;
}
