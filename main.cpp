#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <iomanip>
#include <Antilatency.DeviceNetwork.h>
#include <Antilatency.HardwareExtensionInterface.h>
#include <Antilatency.InterfaceContract.LibraryLoader.h>
#include <Antilatency.HardwareExtensionInterface.Interop.h>

enum class Sides {
    Top,
    Bottom,
    NoConnection,
    ShortCircuit
};

struct IOPins {
    Antilatency::HardwareExtensionInterface::Interop::Pins H_AXIS;
    Antilatency::HardwareExtensionInterface::Interop::Pins V_AXIS;
    Antilatency::HardwareExtensionInterface::Interop::Pins STATUS1;
    Antilatency::HardwareExtensionInterface::Interop::Pins STATUS2;
    Antilatency::HardwareExtensionInterface::Interop::Pins FUNC1;
    Antilatency::HardwareExtensionInterface::Interop::Pins FUNC2;
    Antilatency::HardwareExtensionInterface::Interop::Pins CLICK;
};

Sides SideCheck(Antilatency::HardwareExtensionInterface::ICotask cotask) {
    auto button1 = cotask.createInputPin(Antilatency::HardwareExtensionInterface::Interop::Pins::IO1);
    auto button2 = cotask.createInputPin(Antilatency::HardwareExtensionInterface::Interop::Pins::IO6);
    cotask.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto state1 = button1.getState();
    auto state2 = button2.getState();

    if (state1 == Antilatency::HardwareExtensionInterface::Interop::PinState::Low && state2 == Antilatency::HardwareExtensionInterface::Interop::PinState::Low) {
        std::cout << "ShortCircuit" << std::endl;
        return Sides::ShortCircuit;
    }
    if (state1 == Antilatency::HardwareExtensionInterface::Interop::PinState::Low && state2 == Antilatency::HardwareExtensionInterface::Interop::PinState::High) {
        std::cout << "Side: Top" << std::endl;
        return Sides::Top;
    }
    if (state1 == Antilatency::HardwareExtensionInterface::Interop::PinState::High && state2 == Antilatency::HardwareExtensionInterface::Interop::PinState::Low) {
        std::cout << "Side: Bottom" << std::endl;
        return Sides::Bottom;
    }
    if (state1 == Antilatency::HardwareExtensionInterface::Interop::PinState::High && state2 == Antilatency::HardwareExtensionInterface::Interop::PinState::High) {
        std::cout << "No Connection" << std::endl;
        return Sides::NoConnection;
    }
    return Sides::NoConnection; // Default case
}

std::string sideToString(Sides side) {
    switch (side) {
    case Sides::Top: return "Top";
    case Sides::Bottom: return "Bottom";
    case Sides::NoConnection: return "NoConnection";
    case Sides::ShortCircuit: return "ShortCircuit";
    default: return "Unknown";
    }
}

IOPins Config(Sides side) {
    IOPins ioPins;

    switch (side) {
    case Sides::Top:
        ioPins.STATUS1 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO6;
        ioPins.STATUS2 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO1;
        ioPins.FUNC1 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO5;
        ioPins.FUNC2 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO2;
        ioPins.H_AXIS = Antilatency::HardwareExtensionInterface::Interop::Pins::IOA4;
        ioPins.V_AXIS = Antilatency::HardwareExtensionInterface::Interop::Pins::IOA3;
        ioPins.CLICK = Antilatency::HardwareExtensionInterface::Interop::Pins::IO7;
        break;
    case Sides::Bottom:
        ioPins.STATUS1 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO1;
        ioPins.STATUS2 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO6;
        ioPins.FUNC1 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO2;
        ioPins.FUNC2 = Antilatency::HardwareExtensionInterface::Interop::Pins::IO5;
        ioPins.H_AXIS = Antilatency::HardwareExtensionInterface::Interop::Pins::IOA3;
        ioPins.V_AXIS = Antilatency::HardwareExtensionInterface::Interop::Pins::IOA4;
        ioPins.CLICK = Antilatency::HardwareExtensionInterface::Interop::Pins::IO8;
        break;
    case Sides::NoConnection:
    case Sides::ShortCircuit:
        break;
    }

    return ioPins;
}

void Run(Antilatency::HardwareExtensionInterface::ICotask cotask, IOPins conf, std::string side) {
    auto ledRed = cotask.createPwmPin(conf.STATUS1, 1000, 0);
    auto ledGreen = cotask.createOutputPin(conf.STATUS2, Antilatency::HardwareExtensionInterface::Interop::PinState::High);
    auto hAxis = cotask.createAnalogPin(conf.H_AXIS, 10);
    auto vAxis = cotask.createAnalogPin(conf.V_AXIS, 10);
    auto func1 = cotask.createInputPin(conf.FUNC1);
    auto func2 = cotask.createInputPin(conf.FUNC2);
    auto click = cotask.createInputPin(conf.CLICK);

    cotask.run();

    while (!cotask.isTaskFinished()) {
        std::cout << "Side: " << side << std::endl
            << "HAxis: " << std::fixed << std::setprecision(2) << std::setw(4) << std::right << std::round(hAxis.getValue() * 100.0) / 100.0
            << " VAxis: " << std::setw(4) << std::right << std::round(vAxis.getValue() * 100.0) / 100.0
            << " func1: " << std::setw(4) << std::right << Antilatency::enumToString(func1.getState())
            << " func2: " << std::setw(4) << std::right << Antilatency::enumToString(func2.getState())
            << " click: " << std::setw(4) << std::right << Antilatency::enumToString(click.getState()) << std::endl;

        ledRed.setDuty(hAxis.getValue() * 0.4f);

        if (round(vAxis.getValue() * 100) / 100 >= 2) {
            ledGreen.setState(Antilatency::HardwareExtensionInterface::Interop::PinState::High);
        }
        else {
            ledGreen.setState(Antilatency::HardwareExtensionInterface::Interop::PinState::Low);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::cout << "\033[" << 1 << ";" << 0 << "H"; // Moves the cursor to the first row and first column (top-left corner)
    }
    cotask = {};
}

int main() {
    std::string libNameADN = "AntilatencyDeviceNetwork";
    std::string libNameA = "AntilatencyHardwareExtensionInterface";

    auto deviceNetworkLibrary = Antilatency::InterfaceContract::getLibraryInterface<Antilatency::DeviceNetwork::ILibrary>(libNameADN.c_str());
    if (!deviceNetworkLibrary) {
        std::cerr << "Failed to load Antilatency Device Network library" << std::endl;
        return 1;
    }

    auto networkFilter = deviceNetworkLibrary.createFilter();
    networkFilter.addUsbDevice(Antilatency::DeviceNetwork::Constants::AllUsbDevices);
    auto network = deviceNetworkLibrary.createNetwork(networkFilter);

    auto aheiLibrary = Antilatency::InterfaceContract::getLibraryInterface<Antilatency::HardwareExtensionInterface::ILibrary>(libNameA.c_str());
    if (!aheiLibrary) {
        std::cerr << "Failed to load Antilatency Hardware Extension Interface library" << std::endl;
        return 1;
    }

    auto cotaskConstructor = aheiLibrary.getCotaskConstructor();

    auto targetNode = Antilatency::DeviceNetwork::NodeHandle::Null;

    while (targetNode == Antilatency::DeviceNetwork::NodeHandle::Null) {
        auto supportedNodes = cotaskConstructor.findSupportedNodes(network);
        for (auto node : supportedNodes) {
            if (network.nodeGetStringProperty(node, "Tag") == "ExBoard") {
                targetNode = node;
                break;
            }
        }
        std::cout << "Nodes count " << supportedNodes.size() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto cotask = cotaskConstructor.startTask(network, targetNode);
    auto side = SideCheck(cotask);
    auto conf = Config(side);
    std::string sideString = sideToString(side);
    cotask = {};

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "\x1B[2J\x1B[H"; // Clears the terminal screen and moves the cursor to the top-left corner (home position)

        cotask = cotaskConstructor.startTask(network, targetNode);
        Run(cotask, conf, sideString);
        std::cout << "\x1B[2J\x1B[H"; // Clears the terminal screen and moves the cursor to the top-left corner (home position)

        targetNode = Antilatency::DeviceNetwork::NodeHandle::Null;

        while (targetNode == Antilatency::DeviceNetwork::NodeHandle::Null) {
            auto supportedNodes = cotaskConstructor.findSupportedNodes(network);
            for (auto node : supportedNodes) {
                if (network.nodeGetStringProperty(node, "Tag") == "ExBoard") {
                    targetNode = node;
                    break;
                }
            }
            std::cout << "Invalid Note! Connect the proper device. " << std::endl;
            std::cout << "\033[" << 0 << ";" << 0 << "H"; // Moves the cursor to the top-left corner (home position)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}