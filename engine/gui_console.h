#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <list>

class gui_console
{
	private:
		std::vector< std::string > screen_buffer;
		std::string command_buffer;
		int cursor_position;
		std::vector< std::string > command_history;
		
		int command_history_index;
		int display_start;
		int scroll_offset;
		int w, h;
		bool is_hidden;
		bool is_active;
		
		int current_raster_line;
		const std::string prompt;
		void render_text_line(const std::string& line);

	public:
		gui_console();
		~gui_console();
		bool get_is_hidden();
		void set_is_hidden(bool new_val);
		bool get_is_active();
		void set_is_active(bool new_val);

		void set_dimensions(int w, int h);
		void render();
		void handle_key(char key);
		void print(unsigned char c);
		void print(const std::string& str);
		void scroll(int rows);
		void scroll_abs(int row);
		void last_command();
		void next_command();
		void cursor_left();
		void cursor_right();
		void cursor_home();
		void cursor_end();
		template<typename T> gui_console& operator<<(const T& t)
		{
			std::stringstream text;
			std::cerr << t;
			std::cerr.flush();
			text << t;
			print(text.str());
			return (*this);
		}
};

