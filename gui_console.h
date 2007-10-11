#pragma once

#include <string>
#include <vector>

class gui_console
{
	private:
		std::vector< std::string > screen_buffer;
		std::string command_buffer;
		std::vector< std::string > command_history;
		
		int command_history_index;
		int display_start;
		int scroll_offset;
		int w, h;
		bool is_hidden;
	public:
		gui_console();
		~gui_console();
		bool get_is_hidden();

		void set_dimensions(int w, int h);
		void render();
		void handle_key(char key);
		void print(char c);
		void print(std::string str);
		void scroll(int rows);
		void scroll_abs(int row);
		void last_command();
		void next_command();

};

