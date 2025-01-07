#include "Channel.hpp"

Channel::Channel() : _name(""), _topic(""), _inviteOnly(false), 
					 _topicRestricted(false), _password(""), 
					 _userLimit(-1)
{}

Channel::Channel(const std::string &name)
: _name(name), _topic(""), _inviteOnly(false), 
  _topicRestricted(false), _password(""), _userLimit(-1)
{}

void Channel::addMember(int fd, bool op)
{
	_members[fd] = op;
}

void Channel::removeMember(int fd)
{
	_members.erase(fd);
}

bool Channel::isMember(int fd) const
{
	return (_members.find(fd) != _members.end());
}

bool Channel::isOperator(int fd) const
{
	std::map<int, bool>::const_iterator it = _members.find(fd);
	if (it == _members.end())
		return false;
	return it->second;
}

void Channel::setOperator(int fd, bool op)
{
	_members[fd] = op;
}
