#include <ft_irc.hpp>

std::string TOPIC(std::vector<std::string> args,
                  Socket<sockaddr_in> &from_socket, IRC<sockaddr_in> &irc) {

  Client<sockaddr_in> *client = irc.getClient(from_socket.getFd());
  if (!irc.isRegistered(client)) {
    return ERR_NOTREGISTERED;
  }

  std::string nick = client->user.nickname;
  std::string user = client->user.username;

  if (args.size() < 1)
    return ERR_NEEDMOREPARAMS(nick, "TOPIC");

  std::string channelName = args[0];
  Channel<sockaddr_in> *channel = irc.getChannel(channelName);

  if (!channel)
    return (ERR_NOSUCHCHANNEL(nick, channelName));

  Channel<sockaddr_in> &channelRef = *channel;
  if (!channelRef.isClientInChannel(client))
    return (ERR_NOTONCHANNEL(nick, channelName));

  if (args.size() == 1) {
    std::string channeltopic = channelRef.getTopic();
    if (channeltopic.empty())
      return (RPL_NOTOPIC(nick, channelName));
    return (
        RPL_TOPIC(nick, channelName,
                  channeltopic));
  }

  if (!channelRef.isOperator(client) && channelRef.isTopicOPOnly())
    return (ERR_CHANOPRIVSNEEDED(nick, channelName));

  std::string newTopic;

  for (size_t i = 1; i < args.size(); i++) {
    if (i != 1)
      newTopic.append(" ");
    newTopic.append(args[i]);
  }

  channel->setTopic(newTopic);

  channelRef.broadcast(*client, MSG_TOPIC(nick, user, channelName, newTopic));

  return MSG_TOPIC(nick, user, channelName, newTopic);
}
