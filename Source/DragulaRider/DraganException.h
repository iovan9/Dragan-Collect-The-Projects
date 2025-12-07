#pragma once
#include <exception>
#include <string>

class DraganException : public std::exception
{
protected:
	std::string Message;

public:
	DraganException(const std::string& Msg) : Message(Msg) {}
	virtual const char* what() const noexcept override { return Message.c_str(); }
};

// 3 excepții distincte pentru cerință
class StunAlreadyActive : public DraganException
{
public:
	StunAlreadyActive() : DraganException("Error: Dragan is already stunned!") {}
};

class StaminaDepleted : public DraganException
{
public:
	StaminaDepleted() : DraganException("Error: Not enough stamina!") {}
};

class InvalidTeleportRoom : public DraganException
{
public:
	InvalidTeleportRoom() : DraganException("Error: No valid room found for teleport!") {}
};
