#ifndef GPIO_HANDLER_HPP
#define GPIO_HANDLER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

class GPIO {

private:
	int PinNumber;
	std::string Direction;
	void ExportGPIO(int pin);
	void UnexportGPIO(int pin);

public:
	GPIO(int pin, const std::string& direction);
	~GPIO();
	void SetDirection(const std::string& direction);
	void SetValue(int value);
	void BlinkLed();
	void WriteToFile(const std::string& FilePath, const std::string& Data);
};

#endif
