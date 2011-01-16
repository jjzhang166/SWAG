#include "swag.h"
#include "hardcoded_views.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include <iostream>

#include "size_mode_view.h"

class Desktop_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		Vector2 size;
		const Desktop& desktop = dynamic_cast<const Desktop&>(widget);
		return desktop.Get_child()->Request_size();
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Desktop& desktop = dynamic_cast<const Desktop&>(widget);
		Widget* child = desktop.Get_child();

		if(child)
			child->Render();

		std::string tooltip = desktop.Get_tooltip(0,0);
		if(tooltip != "")
		{
			Vector2 p = desktop.Get_position();
			Vector2 s = desktop.Get_size();
			Vector2 tp = desktop.Get_tooltip_position() + Vector2(20, 10);
			ALLEGRO_COLOR text_color = al_map_rgb_f(0, 0, 0);
			ALLEGRO_COLOR bg_color = al_map_rgb_f(0.9, 0.9, 0.5);
			int w = al_get_text_width(font, tooltip.c_str());
			int h = al_get_font_line_height(font);
			if(tp.x+w > p.x+s.x)
			{
				tp.x = p.x+s.x - w;
				tp.y += 10;
				if(tp.y+h > p.y+s.y)
				{
					tp.y -= (20 + h);
				}
			}
			else if(tp.y+h > p.y+s.y)
			{
				tp.y = p.y+s.y - h;
			}

			al_draw_filled_rectangle(tp.x, tp.y, tp.x+w, tp.y+h, bg_color);
			al_draw_text(font, text_color, tp.x, tp.y, 0, tooltip.c_str());
		}
	}
public:
	ALLEGRO_FONT* font;
};

class Box_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		Vector2 size;
		const Box& box = dynamic_cast<const Box&>(widget);
		Widgets widgets = box.Get_widgets();
		for(Widgets::iterator i = widgets.begin(); i != widgets.end(); ++i)
		{
			Vector2 ws = (*i)->Request_size();
			size.y += ws.y;
			if(ws.x>size.x)
				size.x = ws.x;
		}
		return size;
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Box& box = dynamic_cast<const Box&>(widget);
		Widgets widgets = box.Get_widgets();
		for(Widgets::iterator i = widgets.begin(); i != widgets.end(); ++i)
		{
			(*i)->Render();
		}
	}
};

class HBox_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		Vector2 size;
		const Box& box = dynamic_cast<const Box&>(widget);
		Widgets widgets = box.Get_widgets();
		for(Widgets::iterator i = widgets.begin(); i != widgets.end(); ++i)
		{
			Vector2 ws = (*i)->Request_size();
			size.x += ws.x;
			if(ws.y>size.y)
				size.y = ws.y;
		}
		return size;
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Box& box = dynamic_cast<const Box&>(widget);
		Widgets widgets = box.Get_widgets();
		for(Widgets::iterator i = widgets.begin(); i != widgets.end(); ++i)
		{
			(*i)->Render();
		}
	}
};

class Tree_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Tree& tree = dynamic_cast<const Tree&>(widget);
		Vector2 size;
		const std::string& text = tree.Get_text();
		size.x = al_get_text_width(font, text.c_str()) + 6;
		size.y = al_get_font_line_height(font) + 6;
		float lh = size.y/4+6;

		Trees children = tree.Get_children();
		size.x += size.y/2;
		if(tree.Is_open())
		{
			for(Trees::iterator i = children.begin(); i != children.end(); ++i)
			{
				Vector2 ws = (*i)->Request_size();
				size.y += ws.y;
				if(ws.x+lh>size.x)
					size.x = ws.x+lh;
			}
		}
		return size;
	}
	
	virtual float Get_value(int id, const Widget& widget) const
	{
		if(id == Expander::INDENT)
		{
			float sizey = al_get_font_line_height(font) + 6;
			return sizey/4+6;
		}
		if(id == Expander::SELF_WIDTH)
		{
			const Tree& tree = dynamic_cast<const Tree&>(widget);
			const std::string& text = tree.Get_text();
			float x = al_get_text_width(font, text.c_str()) + 6;
			float y = al_get_font_line_height(font) + 6;
			x += y/2;
			return x;
		}
		if(id == Expander::SELF_HEIGHT)
		{
			return al_get_font_line_height(font) + 6;
		}
		return 0;
	}

	virtual void Render(const Widget& widget) const
	{
		const Tree& tree = dynamic_cast<const Tree&>(widget);

		Trees children = tree.Get_children();
		if(tree.Is_open())
		{
			for(Trees::iterator i = children.begin(); i != children.end(); ++i)
			{
				(*i)->Render();
			}
		}

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		ALLEGRO_COLOR text_color = al_map_rgb_f(1, 1, 1);
		ALLEGRO_COLOR tri_color = al_map_rgb_f(1, 1, 1);
		ALLEGRO_COLOR edge_color = al_map_rgb_f(0.5, 0.5, 0.5);
		ALLEGRO_COLOR select_color = al_map_rgb_f(0.0, 0, 0.8);

		const std::string& text = tree.Get_text();
		float h = al_get_font_line_height(font);

		float text_width = al_get_text_width(font, text.c_str());
		if(tree.Is_selected())
			al_draw_filled_rectangle(p.x+3+h/2, p.y+1, p.x+6+h/2+text_width, p.y+h+6, select_color);

		al_draw_text(font, text_color, p.x+6+h/2, p.y+3, 0, text.c_str());
		if(!children.empty())
		{
			float top = h*0.25;
			float middle = h*0.5;
			float bottom = h*0.75;

			if(tree.Is_open())
			{
				al_draw_filled_triangle(p.x+3, p.y+3+middle, p.x+3+middle, p.y+3+middle, p.x+3+top, p.y+3+bottom, tri_color);
				al_draw_triangle       (p.x+3, p.y+3+middle, p.x+3+middle, p.y+3+middle, p.x+3+top, p.y+3+bottom, edge_color, 0);
			}
			else
			{
				al_draw_filled_triangle(p.x+3, p.y+3+top, p.x+3+top, p.y+3+middle, p.x+3, p.y+3+bottom, tri_color);
				al_draw_triangle       (p.x+3, p.y+3+top, p.x+3+top, p.y+3+middle, p.x+3, p.y+3+bottom, edge_color, 0);
			}
		}
	}
public:
	ALLEGRO_FONT* font;
};

class Expander_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Expander& expander = dynamic_cast<const Expander&>(widget);
		Vector2 size;
		const std::string& text = expander.Get_text();
		size.x = al_get_text_width(font, text.c_str()) + 6;
		size.y = al_get_font_line_height(font) + 6;
		float lh = size.y/4+6;

		Widget* child = expander.Get_child();
		size.x += size.y/2;
		if(child && expander.Is_open())
		{
			Vector2 ws = child->Request_size();
			size.y += ws.y;
			if(ws.x+lh>size.x)
				size.x = ws.x+lh;
		}
		return size;
	}
	
	virtual float Get_value(int id, const Widget& widget) const
	{
		if(id == Expander::INDENT)
		{
			float sizey = al_get_font_line_height(font) + 6;
			return sizey/4+6;
		}
		if(id == Expander::SELF_WIDTH)
		{
			const Expander& expander = dynamic_cast<const Expander&>(widget);
			const std::string& text = expander.Get_text();
			float x = al_get_text_width(font, text.c_str()) + 6;
			float y = al_get_font_line_height(font) + 6;
			x += y/2;
			return x;
		}
		if(id == Expander::SELF_HEIGHT)
		{
			return al_get_font_line_height(font) + 6;
		}
		return 0;
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Expander& expander = dynamic_cast<const Expander&>(widget);

		Widget *child = expander.Get_child();
		if(child && expander.Is_open())
		{
			child->Render();
		}

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		ALLEGRO_COLOR text_color = al_map_rgb_f(1, 1, 1);
		ALLEGRO_COLOR tri_color = al_map_rgb_f(1, 1, 1);
		ALLEGRO_COLOR edge_color = al_map_rgb_f(0.5, 0.5, 0.5);
		ALLEGRO_COLOR select_color = al_map_rgb_f(0.0, 0, 0.8);

		const std::string& text = expander.Get_text();
		float h = al_get_font_line_height(font);

		float text_width = al_get_text_width(font, text.c_str());

		al_draw_text(font, text_color, p.x+6+h/2, p.y+3, 0, text.c_str());
//		if(child)
		{
			float top = h*0.25;
			float middle = h*0.5;
			float bottom = h*0.75;

			if(expander.Is_open())
			{
				al_draw_filled_triangle(p.x+3, p.y+3+middle, p.x+3+middle, p.y+3+middle, p.x+3+top, p.y+3+bottom, tri_color);
				al_draw_triangle       (p.x+3, p.y+3+middle, p.x+3+middle, p.y+3+middle, p.x+3+top, p.y+3+bottom, edge_color, 0);
			}
			else
			{
				al_draw_filled_triangle(p.x+3, p.y+3+top, p.x+3+top, p.y+3+middle, p.x+3, p.y+3+bottom, tri_color);
				al_draw_triangle       (p.x+3, p.y+3+top, p.x+3+top, p.y+3+middle, p.x+3, p.y+3+bottom, edge_color, 0);
			}
		}
	}
public:
	ALLEGRO_FONT* font;
};

class Vertical_paned_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Vertical_paned& vertical_paned = dynamic_cast<const Vertical_paned&>(widget);
		Widget* top = vertical_paned.Get_top();
		Widget* bottom = vertical_paned.Get_bottom();

		Vector2 size;
		Widget_view* view;
		if(top)
		{
			Vector2 ws = top->Request_size();
			size.y += ws.y;
			if(ws.x>size.x)
				size.x = ws.x;
		}
		if(bottom)
		{
			Vector2 ws = bottom->Request_size();
			size.y += ws.y;
			if(ws.x>size.x)
				size.x = ws.x;
		}
		size.y+=7;
		return size;
	}
	
	virtual float Get_value(int id, const Widget& widget) const
	{
		return 7;
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Vertical_paned& vertical_paned = dynamic_cast<const Vertical_paned&>(widget);
		Widget* top = vertical_paned.Get_top();
		Widget* bottom = vertical_paned.Get_bottom();
		Widget_view* view;
		int clip_x, clip_y, clip_w, clip_h;
		al_get_clipping_rectangle(&clip_x, &clip_y, &clip_w, &clip_h);
		if(top)
		{
			Vector2 p = top->Get_position();
			Vector2 s = top->Get_size();
			al_set_clipping_rectangle(p.x, p.y, s.x, s.y);
			top->Render();
		}
		if(bottom)
		{
			Vector2 p = bottom->Get_position();
			Vector2 s = bottom->Get_size();
			al_set_clipping_rectangle(p.x, p.y, s.x, s.y);
			bottom->Render();
		}
		al_set_clipping_rectangle(clip_x, clip_y, clip_w, clip_h);

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		float pane = p.y+vertical_paned.Get_pane_position()+1;
		ALLEGRO_COLOR bg_color = al_map_rgb_f(0.7, 0.7, 0.7);
		ALLEGRO_COLOR edge_color = al_map_rgb_f(0.3, 0.3, 0.3);
		al_draw_filled_rectangle(p.x, pane, p.x+s.x-1, pane+6, bg_color);
		al_draw_rectangle(p.x, pane, p.x+s.x-1, pane+6, edge_color, 0);
		al_draw_line(p.x+s.x/2-10, pane+2, p.x+s.x/2+10, pane+2, edge_color, 0);
		al_draw_line(p.x+s.x/2-10, pane+4, p.x+s.x/2+10, pane+4, edge_color, 0);
	}
};


class Horizontal_paned_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Horizontal_paned& horizontal_paned = dynamic_cast<const Horizontal_paned&>(widget);
		Widget* left = horizontal_paned.Get_left();
		Widget* right = horizontal_paned.Get_right();

		Vector2 size;
		Widget_view* view;
		if(left)
		{
			Vector2 ws = left->Request_size();
			size.x += ws.x;
			if(ws.y>size.y)
				size.y = ws.y;
		}
		if(right)
		{
			Vector2 ws = right->Request_size();
			size.x += ws.x;
			if(ws.y>size.y)
				size.y = ws.y;
		}
		size.x+=7;
		return size;
	}
	
	virtual float Get_value(int id, const Widget& widget) const
	{
		return 7;
	}

	virtual void Render(const Widget& widget) const
	{
		const Horizontal_paned& horizontal_paned = dynamic_cast<const Horizontal_paned&>(widget);
		Widget* left = horizontal_paned.Get_left();
		Widget* right = horizontal_paned.Get_right();
		Widget_view* view;
		int clip_x, clip_y, clip_w, clip_h;
		al_get_clipping_rectangle(&clip_x, &clip_y, &clip_w, &clip_h);
		if(left)
		{
			Vector2 p = left->Get_position();
			Vector2 s = left->Get_size();
			al_set_clipping_rectangle(p.x, p.y, s.x, s.y);
			left->Render();
		}
		if(right)
		{
			Vector2 p = right->Get_position();
			Vector2 s = right->Get_size();
			al_set_clipping_rectangle(p.x, p.y, s.x, s.y);
			right->Render();
		}
		al_set_clipping_rectangle(clip_x, clip_y, clip_w, clip_h);

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		float pane = p.x+horizontal_paned.Get_pane_position();
		ALLEGRO_COLOR bg_color = al_map_rgb_f(0.7, 0.7, 0.7);
		ALLEGRO_COLOR edge_color = al_map_rgb_f(0.3, 0.3, 0.3);
		al_draw_filled_rectangle(pane, p.y+1, pane+6, p.y+s.y, bg_color);
		al_draw_rectangle(pane, p.y+1, pane+6, p.y+s.y, edge_color, 0);
		al_draw_line(pane+2, p.y+s.y/2-10, pane+2, p.y+s.y/2+10, edge_color, 0);
		al_draw_line(pane+4, p.y+s.y/2-10, pane+4, p.y+s.y/2+10, edge_color, 0);
	}
};

class Button_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Button& button = dynamic_cast<const Button&>(widget);
		const std::string& text = button.Get_text();
		Vector2 size;
		size.x = al_get_text_width(font, text.c_str()) + 6;
		size.y = al_get_font_line_height(font) + 6;
		return size;
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Button& button = dynamic_cast<const Button&>(widget);

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		ALLEGRO_COLOR text_color = al_map_rgb_f(0, 0, 0);
		ALLEGRO_COLOR bg_color = al_map_rgb_f(1, 1, 1);
		ALLEGRO_COLOR edge_color = al_map_rgb_f(0.5, 0.5, 0.5);
		float h = al_get_font_line_height(font);

		if(button.Is_pressed() && button.Is_mouse_over())
		{
			al_draw_filled_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, edge_color);
			al_draw_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, bg_color, 0);
		}
		else
		{
			al_draw_filled_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, bg_color);
			al_draw_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, edge_color, 0);
		}

		int font_h = al_get_font_line_height(font);
		int y = p.y + (s.y - font_h)/2;
		int x = p.x + s.x/2;
		const std::string& text = button.Get_text();
		al_draw_text(font, text_color, x, y, ALLEGRO_ALIGN_CENTRE, text.c_str());
	}
public:
	ALLEGRO_FONT* font;
};

class Inputbox_view: public Widget_view
{
public:
	Inputbox_view()
	:cursor_flash(0)
	{}

	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Inputbox& inputbox = dynamic_cast<const Inputbox&>(widget);
		std::string text = inputbox.Get_text();
		Vector2 size;
		size.x = al_get_text_width(font, text.c_str()) + 6;
		size.y = al_get_font_line_height(font) + 6;
		return size;
	}
	
	virtual float Get_value(int id, const Widget& widget) const
	{
		const Inputbox& inputbox = dynamic_cast<const Inputbox&>(widget);
		std::string text = inputbox.Get_text();
		ALLEGRO_MOUSE_STATE mouse;
		al_get_mouse_state(&mouse);
		int char_w = al_get_text_width(font, " ");
		int x = mouse.x - widget.Get_position().x-6;
		int guess = x/char_w;
		int diff = x-al_get_text_width(font, text.substr(0, guess).c_str());
		//First back up if needed
		while(diff<0 && guess > 0)
		{
			--guess;
			diff = x-al_get_text_width(font, text.substr(0, guess).c_str());
		}
		//Then check forth
		int diff2;
		while(diff>0 && guess < text.length())
		{
			diff2 = diff;
			++guess;
			diff = x-al_get_text_width(font, text.substr(0, guess).c_str());
		}
		if(diff>diff2)
			--guess;
		if(guess<0)
			guess=0;
		if(guess>text.length())
			guess=text.length();
		return guess;
	}

	virtual void Set_value(int id, float v)
	{
		cursor_flash=0;
	}

	virtual void Render(const Widget& widget) const
	{
		const Inputbox& inputbox = dynamic_cast<const Inputbox&>(widget);

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		ALLEGRO_COLOR text_color = al_map_rgb_f(0, 0, 0);
		ALLEGRO_COLOR bg_color = al_map_rgb_f(1, 1, 1);
		ALLEGRO_COLOR edge_color = al_map_rgb_f(0.5, 0.5, 0.5);

		al_draw_filled_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, bg_color);
		al_draw_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, edge_color, 0);

		int y = p.y + 3;
		int x = p.x + 3;
		std::string text = inputbox.Get_text();
		
		if(inputbox.Has_focus())
		{
			int sel_s = inputbox.Get_selection_start();
			int sel_e = inputbox.Get_selection_end();

			int cp_s = al_get_text_width(font, text.substr(0, sel_s).c_str());
			int cp_e = al_get_text_width(font, text.substr(0, sel_e).c_str());
			int h = al_get_font_line_height(font);
			if(sel_s != sel_e)
				al_draw_filled_rectangle(x+cp_s-1, y, x+cp_e, y+h, al_map_rgb_f(0.5, 0.5, 1));
			else if(cursor_flash>0)
				al_draw_line(x+cp_e-1, y, x+cp_e, y+h, al_map_rgb_f(0, 0, 0), 0);
		}

		al_draw_text(font, text_color, x, y, 0, text.c_str());
	}

	virtual void Update(float t)
	{
		cursor_flash+=t;
		if(cursor_flash>0.5)
			cursor_flash-=1;
	}
public:
	ALLEGRO_FONT* font;
	float cursor_flash;
};
class Label_view: public Widget_view
{
public:
	virtual Vector2 Request_size(const Widget& widget) const
	{
		const Label& label = dynamic_cast<const Label&>(widget);
		std::string text = label.Get_text();
		Vector2 size;
		size.x = al_get_text_width(font, text.c_str()) + 6;
		size.y = al_get_font_line_height(font) + 6;
		return size;
	}
	
	virtual void Render(const Widget& widget) const
	{
		const Label& label = dynamic_cast<const Label&>(widget);

		Vector2 p = widget.Get_position();
		Vector2 s = widget.Get_size();
		ALLEGRO_COLOR text_color = al_map_rgb_f(0, 0, 0);
		ALLEGRO_COLOR bg_color = al_map_rgb_f(1, 1, 1);

		al_draw_filled_rectangle(p.x, p.y+1, p.x+s.x-1, p.y+s.y, bg_color);

		int y = p.y + 3;
		int x = p.x + 3;
		std::string text = label.Get_text();
		al_draw_text(font, text_color, x, y, 0, text.c_str());
	}
public:
	ALLEGRO_FONT* font;
};

Hardcoded_skin::Hardcoded_skin()
{
	font = al_load_font("data/times.ttf", 12, 0);
	if(!font)
		font = al_load_font("examples/data/times.ttf", 12, 0);
	
	Widget* widget;

	Size_mode_view* size_mode_view = new Size_mode_view;
	size_mode_view->font = font;
	widget = new Widget;
	widget->Set_view(size_mode_view);
	Set_prototype("size mode", widget);
	Add_view(size_mode_view);

	Horizontal_paned_view* hpaned_view = new Horizontal_paned_view;
	widget = new Horizontal_paned;
	widget->Set_view(hpaned_view);
	Set_prototype("horizontal paned", widget);
	Add_view(hpaned_view);

	Vertical_paned_view* vpaned_view = new Vertical_paned_view;
	widget = new Vertical_paned;
	widget->Set_view(vpaned_view);
	Set_prototype("vertical paned", widget);
	Add_view(vpaned_view);

	Desktop_view* desktop_view = new Desktop_view;
	desktop_view->font = font;
	widget = new Desktop;
	widget->Set_view(desktop_view);
	Set_prototype("desktop", widget);
	Add_view(desktop_view);

	Box_view* vbox_view = new Box_view;
	widget = new Vertical_box;
	widget->Set_view(vbox_view);
	Set_prototype("vertical box", widget);
	Add_view(vbox_view);

	HBox_view* hbox_view = new HBox_view;
	widget = new Horizontal_box;
	widget->Set_view(hbox_view);
	Set_prototype("horizontal box", widget);
	Add_view(hbox_view);

	Tree_view* tree_view = new Tree_view;
	tree_view->font = font;
	widget = new Tree;
	widget->Set_view(tree_view);
	Set_prototype("tree", widget);
	Add_view(tree_view);

	Expander_view* expander_view = new Expander_view;
	expander_view->font = font;
	widget = new Expander;
	widget->Set_view(expander_view);
	Set_prototype("expander", widget);
	Add_view(expander_view);

	Button_view* button_view = new Button_view;
	button_view->font = font;
	widget = new Button;
	widget->Set_view(button_view);
	Set_prototype("button", widget);
	Add_view(button_view);

	Label_view* label_view = new Label_view;
	label_view->font = font;
	widget = new Label;
	widget->Set_view(label_view);
	Set_prototype("label", widget);
	Add_view(label_view);

	Inputbox_view* inputbox_view = new Inputbox_view;
	inputbox_view->font = font;
	widget = new Inputbox;
	widget->Set_view(inputbox_view);
	Set_prototype("inputbox", widget);
	Add_view(inputbox_view);
	Add_animated_view(inputbox_view);
}

Hardcoded_skin::~Hardcoded_skin()
{
	al_destroy_font(font);
}