#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>

class Channel
{
private:
	std::string         _name;
	std::string         _topic;
	std::map<int, bool> _members; // fd -> isOperator
	bool                _inviteOnly;
	bool                _topicRestricted;
	std::string         _password; // for 'k' mode
	int                 _userLimit; // for 'l' mode

public:
	Channel();
	Channel(const std::string &name);

	void setTopic(const std::string &topic)         { _topic = topic; }
	void setInviteOnly(bool invite)                 { _inviteOnly = invite; }
	void setTopicRestricted(bool restricted)        { _topicRestricted = restricted; }
	void setPassword(const std::string &pass)       { _password = pass; }
	void setUserLimit(int limit)                    { _userLimit = limit; }

	std::string getName() const                     { return _name; }
	std::string getTopic() const                    { return _topic; }
	std::string getPassword() const                 { return _password; }
	int         getUserLimit() const                { return _userLimit; }
	bool        isInviteOnly() const                { return _inviteOnly; }
	bool        isTopicRestricted() const           { return _topicRestricted; }

	void addMember(int fd, bool op = false);
	void removeMember(int fd);
	bool isMember(int fd) const;
	bool isOperator(int fd) const;
	void setOperator(int fd, bool op);

	const std::map<int, bool> &getMembers() const   { return _members; }
};

#endif
