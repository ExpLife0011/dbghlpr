#include "code_graph.h"

void code_graph::tree::set_root(code_graph::node *n)
{
	if (!find(n->get_ogdf_node()))
	{
		node_list_.push_back(n);
		odgf_map_[n->get_ogdf_node()] = n;

		if (n->get_left())
		{
			set_root(n->get_left());
		}
		if (n->get_right())
		{
			set_root(n->get_right());
		}
	}
}

code_graph::node *code_graph::tree::find(ogdf::node n)
{
	std::map<ogdf::node, code_graph::node *>::iterator it = odgf_map_.find(n);
	if (it == odgf_map_.end())
	{
		return nullptr;
	}
	else
	{
		return it->second;
	}
}

code_graph::node *code_graph::tree::find(QString str)
{
	std::list<code_graph::node *>::iterator it = node_list_.begin();
	for (it; it != node_list_.end(); ++it)
	{
		code_graph::node *n = *it;
		if (strstr(n->get_data().toStdString().c_str(), str.toStdString().c_str()))
		{
			return n;
		}
		else if (strstr(n->get_left()->get_data().toStdString().c_str(), str.toStdString().c_str()))
		{
			return n->get_left();
		}
		else if (strstr(n->get_right()->get_data().toStdString().c_str(), str.toStdString().c_str()))
		{
			return n->get_right();
		}
	}

	return nullptr;
}
