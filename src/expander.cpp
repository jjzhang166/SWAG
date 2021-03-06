#include "expander.h"
#include "widget_view.h"
#include "event_queue.h"
#include <yaml-cpp/yaml.h>

Expander::Expander()
:open(false)
,child(NULL)
{
}

Widget* Expander::Clone() const
{
	return new Expander(*this);
}

bool Expander::Is_open() const
{
	return open;
}

void Expander::Open()
{
	if(!open)
	{
		open = true;
		Calculate_request_size();
	}
}

void Expander::Close()
{
	if(open)
	{
		open = false;
		Calculate_request_size();
	}
}

void Expander::Set_text(const Ustring& t)
{
	text = t;
	Calculate_request_size();
}

const Ustring& Expander::Get_text() const
{
	return text;
}

Widget* Expander::Get_child() const
{
	return child;
}

void Expander::Organise()
{
	float indent = Get_value(INDENT);
	Vector2 p = Get_position();
	p.y += Get_value(SELF_HEIGHT);
	Vector2 s = Get_size();
	s.y -= Get_value(SELF_HEIGHT);

	if(child)
	{
		child->Set_size(s);
		child->Set_position(p);
	}
	Calculate_request_size();
}

void Expander::Handle_event(const ALLEGRO_EVENT& event)
{
	if(child && Is_open())
		child->Handle_event(event);

	if(ALLEGRO_EVENT_MOUSE_BUTTON_UP == event.type)
	{
		Vector2 p = Get_position();
		bool covers_point = Covers_point(event.mouse.x, event.mouse.y);

		if(covers_point && event.mouse.y<p.y+Get_value(SELF_HEIGHT))
		{
			if(event.mouse.x < p.x+Get_value(INDENT))
			{
				if(Is_open())
					Close();
				else
					Open();
			}
		}
	}
}

void Expander::Resized()
{
	Organise();
}

void Expander::Handle_child_resize()
{
	Resized();
}

bool Expander::Add_child(Widget* c)
{
	if(!child)
	{
		child = c;
		child->Set_parent(this);
		Organise();
		return true;
	}
	return false;
}

void Expander::Remove_child(Widget* c)
{
	child = NULL;
	Organise();
}

Widgets Expander::Get_children() const
{
	Widgets w;
	if(child) {
		w.push_back(child);
	}
	return w;
}

const Ustring& Expander::Get_tooltip(float x, float y) const
{
	if(child && child->Covers_point(x, y))
		return child->Get_tooltip(x, y);
	return Widget::Get_tooltip(x, y);
}

void Expander::To_yaml(YAML::Emitter& out) const
{
	Container::To_yaml(out);
	out << YAML::Key << "Expander";
	out << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "text";
		out << YAML::Value << text.Cstring();
	out << YAML::EndMap;

	out << YAML::Key << "children";
	out << YAML::Value << YAML::BeginSeq;
	if(child && child->Get_name() != "")
	{
		out << child->Get_name();
	}

	out << YAML::EndSeq;
}

void Expander::From_yaml(const YAML::Node& in)
{
	Container::From_yaml(in);
	const YAML::Node& doc = in["Expander"];
	doc["text"] >> text;
	std::cout<<"Loaded Expander: "<<text<<std::endl;
	Calculate_request_size();
}
