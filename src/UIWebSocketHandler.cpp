#include "UIWebSocketHandler.h"
#include "settings.h"

#include "ui.pb.h"

#include <iostream>

//--------------------------------------------------------------
UIWebSocketHandler::UIWebSocketHandler(StreamSocket& socket, SocketReactor& reactor)
: WebSocketHandler(socket, reactor)
, bDataToSend(true)
{}

//--------------------------------------------------------------
void UIWebSocketHandler::ready()
{
	if (!bDataToSend)
		return;

	std::string data;

	if (resource == "/")
		std::cout << "Request for / [DENIED]" << std::endl;

	else if (resource == "/gui.pb")
	{
		ui::Parameters parameters_pb;
		ui::Parameters::Parameter* parameter_pb;
		
		parameters_pb.set_name("Arggggh");
		parameters_pb.set_id(12345);

		parameter_pb = parameters_pb.add_parameter();		
		parameter_pb->set_id(43);

		parameters_pb.SerializeToString(&data);
	}
	else {
		std::cout << "Request for " << resource << " [DENIED]" << std::endl;
		return;
	}
	
	//	cout<< "Sending " << frame.data.size() << " bytes" << endl;
	send(data);
	bDataToSend = false;
}

//--------------------------------------------------------------
void
UIWebSocketHandler::idle()
{
	std::cout << "Idle'd" << std::endl;
//	ofSleepMillis(1000);
}

//--------------------------------------------------------------
void
UIWebSocketHandler::onmessage(const std::string& frame)
{
	std::cout<< "Recv'd " << frame.size() << " bytes: " << " from " << service << std::endl;
}

//--------------------------------------------------------------
void
UIWebSocketHandler::onopen()
{}

//--------------------------------------------------------------
void
UIWebSocketHandler::onclose()
{}
