/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2019 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/saland
===========================================================================
*/

#include "../global.hpp"
#include "console/Console.hpp"
#include "../common.h"

struct ConsoleCommandGiveItem : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "give";
	}
	virtual std::string run(const std::vector<std::string>& args) override {
		if (args.size() != 3) {
			throw std::runtime_error("Must be called like: give ITEM_NAME QUANTITY");
		}
		std::string item_name = args.at(1);
		int quantity = 1;
		try {
			quantity = std::stoi(args.at(2));
		}
		catch (...) {
			std::string error_msg = "Failed to convert \"";
			error_msg += args.at(2)+ "\" into an integer";
			throw std::runtime_error(error_msg);
		}
		if (quantity == 0) {
			return std::string("Inventory not touched");
		}
		int prevCount = globalData.player.item_inventory[item_name];
		globalData.player.item_inventory[item_name] += quantity;
		if (quantity > 0) {
			return std::string("Added ")+std::to_string(quantity)+" of "+item_name;
		}
		else {
			if (globalData.player.item_inventory[item_name] < 0) {
				globalData.player.item_inventory[item_name] = 0;
			}
			return std::string("Removed ")+std::to_string(prevCount-globalData.player.item_inventory[item_name])+" of "+item_name;
		}

	}

	virtual std::string helpMessage() const override {
		return "Give item to player. Must be called like: give ITEM_NAME QUANTITY";
	}
};


struct ConsoleCommandQuit : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "quit";
	}
	virtual std::string run(const std::vector<std::string>&) override {
		globalData.isShuttingDown = true;
		return "Exiting...";
	}

	virtual std::string helpMessage() const override {
		return "Exits the game";
	}
};

struct ConsoleCommandConfig : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "config";
	}

	virtual std::string run(const std::vector<std::string>& args) override {
		if (args.size() < 2) {
			throw std::runtime_error("See \"help config\" for calling instructions.");
		}
		if (args[1] == "list") {
			std::string theList;
			const std::map<std::string, std::string>& configs = Config::getInstance()->getConfigPairs();
			for (const auto& item : configs) {
				theList += item.first + "\n";
			}
			return theList;
		}
		else if (args[1] == "get") {
			if (args.size() < 3) {
				throw std::runtime_error("Must be called like \"config get VARIABLE_NAME\". Use \"config list\" to get the names.");
			}
			const std::string& key = args[2];
			if (!Config::getInstance()->exists(key)) {
				throw std::runtime_error(std::string("key \"")+key+"\" not found");
			}
			return Config::getInstance()->getString(key);
		}
		else if (args[1] == "set") {
			if (args.size() < 4) {
				throw std::runtime_error("Must be called like \"config set VARIABLE_NAME NEW_VALUE\". Use \"config list\" to set the value.");
			}
			const std::string& key = args[2];
			const std::string& value = args[3];
			Config::getInstance()->setString(key, value);
			return "Value set";
		}
		throw std::runtime_error("Missing command");
	}

	virtual std::string helpMessage() const override {
		return "Use \"config list\" to list the variables already set";
	}
};

static ConsoleCommandGiveItem cc_give_item;
static ConsoleCommandQuit cc_quit;
static ConsoleCommandConfig cc_config;

void GameConsoleCommandRegister() {
	RegisterCommand(&cc_give_item);
	RegisterCommand(&cc_quit);
	RegisterCommand(&cc_config);
}