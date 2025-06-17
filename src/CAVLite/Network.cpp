#include "Network.h"
#include <set>
#include "config.h"
#include <queue>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <direct.h>  // for _mkdir
#include <sys/stat.h>
#include <cstring>


extern std::ofstream debug_log_file;

void printBackwardTree(int movement_id, MesoLink* p_link);
void validateBackwardTree(int movement_id, MesoLink* p_link);
void dumpBackwardTreeToTXT(int movement_id, MesoLink* p_link);
void dumpTurningMIDNodesToTXT(int movement_id, MesoLink* p_link, Network* net);


void Network::init()
{
	std::cout << "Network initializing...";
	getMicroInOutNodesOfMesolink();
	generateCostTree();
	std::cout << "Done\n";
}


void Network::getMicroInOutNodesOfMesolink()
{
	// --------------micro incoming and outgoing nodes of each meso link------------- //
	MesoLink* p_mesolink;
	MicroLink* p_microlink;
	std::vector<int>::iterator retEndPos;

	for (int i = 0; i < number_of_meso_links; i++)
	{
		p_mesolink = &meso_link_vector[i];

		std::set<int> potential_micro_incoming_node_id_set;
		std::set<int> potential_micro_outgoing_node_id_set;

		for (int j = 0; j < p_mesolink->micro_link_set.size(); j++)
		{
			p_microlink = &micro_link_vector[micro_link_id_to_seq_no_dict[p_mesolink->micro_link_set[j]]];

			potential_micro_incoming_node_id_set.insert(p_microlink->from_node_id);
			potential_micro_outgoing_node_id_set.insert(p_microlink->to_node_id);
		}

		p_mesolink->micro_incoming_node_id_vector.resize(potential_micro_incoming_node_id_set.size());
		p_mesolink->micro_outgoing_node_id_vector.resize(potential_micro_outgoing_node_id_set.size());

		retEndPos = set_difference(potential_micro_incoming_node_id_set.begin(),
			potential_micro_incoming_node_id_set.end(), potential_micro_outgoing_node_id_set.begin(),
			potential_micro_outgoing_node_id_set.end(), p_mesolink->micro_incoming_node_id_vector.begin());
		p_mesolink->micro_incoming_node_id_vector.resize(retEndPos - p_mesolink->micro_incoming_node_id_vector.begin());

		retEndPos = set_difference(potential_micro_outgoing_node_id_set.begin(),
			potential_micro_outgoing_node_id_set.end(), potential_micro_incoming_node_id_set.begin(),
			potential_micro_incoming_node_id_set.end(), p_mesolink->micro_outgoing_node_id_vector.begin());
		p_mesolink->micro_outgoing_node_id_vector.resize(retEndPos - p_mesolink->micro_outgoing_node_id_vector.begin());
	}
}


void Network::generateCostTree()
{
	// ==== Begin: Prepare debug output folder ====
	debug_log_file << "=== Begin Backward Tree Construction Log ===\n";
	_mkdir("output");
	_mkdir("output/debug");
	// ==== End ====


	MesoLink *from_link, *to_link;
	MesoNode *current_node;
	std::vector<int>::iterator ret;

	//
	// For each meso link and its corresponding meso to-node (i.e., the end node of the meso link),
	// we iterate over its outgoing micro nodes, obtained using the getMicroInOutNodesOfMesolink() function.
	// For each of these outgoing micro nodes, we then iterate over the outgoing meso links from the meso to-node.
	// If an outgoing micro node exists in the incoming micro node set of an outgoing meso link,
	// we record it as a valid turning node and store it in the turning_node_seq_no_dict,
	// using the outgoing meso link’s sequence number as the key.


	for (int i = 0; i < number_of_meso_links; i++)
	{
		from_link = &meso_link_vector[i];
		current_node = &meso_node_vector[meso_node_id_to_seq_no_dict[from_link->to_node_id]];

		for (int j = 0; j < from_link->micro_outgoing_node_id_vector.size(); j++)
		{
			int micro_node_id = from_link->micro_outgoing_node_id_vector[j];
			for (int k = 0; k < current_node->m_outgoing_link_vector.size(); k++)
			{
				to_link = &meso_link_vector[meso_link_id_to_seq_no_dict[current_node->m_outgoing_link_vector[k]]];

				ret = std::find(to_link->micro_incoming_node_id_vector.begin(), to_link->micro_incoming_node_id_vector.end(), micro_node_id);
				if (ret != to_link->micro_incoming_node_id_vector.end())
				{
					int micro_node_seq_no = micro_node_id_to_seq_no_dict[micro_node_id];
					if (from_link->turning_node_seq_no_dict.count(to_link->link_seq_no) == 0)
					{
						std::vector<int> node_seq_no_vector;
						node_seq_no_vector.push_back(micro_node_seq_no);
						from_link->turning_node_seq_no_dict[to_link->link_seq_no] = node_seq_no_vector;
					}
					else
					{
						from_link->turning_node_seq_no_dict[to_link->link_seq_no].push_back(micro_node_seq_no);
					}
				}
			}
		}
	}


	// #pragma omp parallel for
	MesoLink *p_link;
	// p_link->micro_node_set is a 2D vector, where the first dimension corresponds to the lane index (i.e., lane number - 1) of the p_link,
	// and the second dimension contains the micro node IDs along that lane.
	//
	// For each meso link, we check if turning_node_seq_no_dict is empty.
	// If it is empty (i.e., the meso link has no outgoing connections), we use -1 as the key in the estimated cost tree
	// to indicate a dead-end movement.
	//
	// The MicroShortestPath() function returns a float array representing the cost to reach a specified micro node destination
	// from other micro nodes (indexed by node sequence number) all within a specified meso link.
	//
	// - If turning_node_seq_no_dict is empty, costs are computed from all outgoing micro nodes (node set) of the current meso link.
	// - If turning_node_seq_no_dict is not empty, each key corresponds to an outgoing meso link,
	//   and its associated vector contains the corresponding MID nodes — specifically, the outgoing micro nodes from the current meso link's
	//   micro outgoing node set that also appear in the incoming micro node set of the outgoing meso link.
	//   These nodes serve as the starting points for building the backward cost tree.


	
	for (int i = 0; i < number_of_meso_links; i++)
	{
		p_link = &meso_link_vector[i];
		if (p_link->turning_node_seq_no_dict.empty())
		{
			//no outgoing link
			std::map<int, float>::iterator iter;
			
			for (int p = 0; p < p_link->micro_node_set.size(); p++)
			{
				for (int q = 0; q < p_link->micro_node_set[p].size(); q++)
				{
					p_link->estimated_cost_tree_for_each_movement[-1][micro_node_id_to_seq_no_dict[p_link->micro_node_set[p][q]]] = _MAX_COST_TREE;
				}
			}

			if (p_link->isconnector)
			{
				for (int q = 0; q < p_link->micro_incoming_node_id_vector.size(); q++)
				{
					p_link->estimated_cost_tree_for_each_movement[-1][micro_node_id_to_seq_no_dict[p_link->micro_incoming_node_id_vector[q]]] = _MAX_COST_TREE;
				}

				for (int q = 0; q < p_link->micro_outgoing_node_id_vector.size(); q++)
				{
					p_link->estimated_cost_tree_for_each_movement[-1][micro_node_id_to_seq_no_dict[p_link->micro_outgoing_node_id_vector[q]]] = _MAX_COST_TREE;
				}
			}

			for (int j = 0; j < p_link->micro_outgoing_node_id_vector.size(); j++)
			{
				float *micro_node_label_cost = MicroShortestPath(micro_node_id_to_seq_no_dict[p_link->micro_outgoing_node_id_vector[j]], p_link);

				iter = p_link->estimated_cost_tree_for_each_movement[-1].begin();
				while (iter != p_link->estimated_cost_tree_for_each_movement[-1].end())
				{
					if (micro_node_label_cost[iter->first] < iter->second)
						iter->second = micro_node_label_cost[iter->first];
					iter++;
				}
			}
		}
		else
		{
			//with outgoing link
			std::map<int, std::vector<int>>::iterator iter;
			std::map<int, float>::iterator iter1;
			iter = p_link->turning_node_seq_no_dict.begin();
			while (iter != p_link->turning_node_seq_no_dict.end())
			{
				for (int p = 0; p < p_link->micro_node_set.size(); p++)
				{
					for (int q = 0; q < p_link->micro_node_set[p].size(); q++)
					{
						p_link->estimated_cost_tree_for_each_movement[iter->first][micro_node_id_to_seq_no_dict[p_link->micro_node_set[p][q]]] = _MAX_COST_TREE;
					}
				}

				if (p_link->isconnector)
				{
					for (int q = 0; q < p_link->micro_incoming_node_id_vector.size(); q++)
					{
						p_link->estimated_cost_tree_for_each_movement[iter->first][micro_node_id_to_seq_no_dict[p_link->micro_incoming_node_id_vector[q]]] = _MAX_COST_TREE;
					}

					for (int q = 0; q < p_link->micro_outgoing_node_id_vector.size(); q++)
					{
						p_link->estimated_cost_tree_for_each_movement[iter->first][micro_node_id_to_seq_no_dict[p_link->micro_outgoing_node_id_vector[q]]] = _MAX_COST_TREE;
					}
				}


				for (int j = 0; j < iter->second.size(); j++)
				{
					float *micro_node_label_cost = MicroShortestPath(iter->second[j], p_link);

					if (!micro_node_label_cost) {
						debug_log_file << "[WARNING] MID node seq_no " << iter->second[j]
							<< " in movement " << iter->first << " on meso link "
								<< p_link->link_id << " returned null cost array (no incoming links).\n";
							//continue; not continue now for debugging
					}

					// ==== Begin: Debug MID node usage ====
					std::cout << "[DEBUG] Using MID nodes for movement " << iter->first
						<< " on meso link " << p_link->link_id << ":\n";
					for (int k = 0; k < iter->second.size(); ++k) {
						int seq_no = iter->second[k];
						//int node_id = net->micro_node_vector[seq_no].node_id;
						int node_id = this->micro_node_vector[seq_no].node_id;

						//std::cout << "  MID node: id = " << node_id << ", seq_no = " << seq_no << "\n";
						debug_log_file << "MID node: id = " << node_id << ", seq_no = " << seq_no << "\n";

					}
					// ==== End: Debug MID node usage ====



					iter1 = p_link->estimated_cost_tree_for_each_movement[iter->first].begin();
					while (iter1 != p_link->estimated_cost_tree_for_each_movement[iter->first].end())
					{
						if (micro_node_label_cost[iter1->first] < iter1->second)
							iter1->second = micro_node_label_cost[iter1->first];
						iter1++;
					}

					delete micro_node_label_cost;
				}

				// ==== Begin: Print and validate backward tree ====
				printBackwardTree(iter->first, p_link);
				validateBackwardTree(iter->first, p_link);
				dumpTurningMIDNodesToTXT(iter->first, p_link, this);
				dumpBackwardTreeToTXT(iter->first, p_link);
				// ==== End: Print and validate backward tree ====

				iter++;
			}

		}

	}
}


//
// Computes a backward shortest path tree (label correcting) for a given micro node destination,
// limited to the micro nodes that belong to the provided meso link.
//
// The function returns an array (float*) of cost labels indexed by micro node sequence number.
// Each value in the array represents the cost to reach the destination node from a specific micro node,
// using only micro links internal to the specified meso link.
//
// - If the destination node has no incoming micro links, returns NULL.
//
// - micro_node_label_cost[i] stores the minimum cost from node i to the destination.
// - The cost is computed using free-flow travel time to the to_node, cost-to-go of the to_node, and additional link cost.
// - Only paths confined to the same meso link (checked via meso_link_id) are considered.
//
// The algorithm works by:
// 1. Initializing all node costs to a large value (_MAX_COST_TREE).
// 2. Setting the destination node cost to 0.
// 3. Using a FIFO queue (SEList) to iteratively expand nodes in a backward direction.
// 4. For each visited node, checking all incoming links.
// 5. If the origin of a link belongs to the same meso link and a lower cost is found,
//    update the label and push the origin node into the queue.
//
// Note: Predecessor arrays for nodes and links are created but only used internally;
// they are deleted before returning the cost array.

float * Network::MicroShortestPath(int destination_node_seq_no, MesoLink * mesolink)
{
	if (micro_node_vector[destination_node_seq_no].m_incoming_link_vector.size() == 0)
		return NULL;

	int *micro_node_predecessor = new int[number_of_micro_nodes];
	float *micro_node_label_cost = new float[number_of_micro_nodes];
	int *micro_link_predecessor = new int[number_of_micro_nodes];

	for (int i = 0; i < number_of_micro_nodes; i++)
	{
		micro_node_predecessor[i] = -1;
		micro_node_label_cost[i] = _MAX_COST_TREE;
		micro_link_predecessor[i] = -1;
	}

	micro_node_label_cost[destination_node_seq_no] = 0;

	std::queue<int> SEList;
	SEList.push(destination_node_seq_no);

	while (!SEList.empty())
	{
		int to_node_seq_no = SEList.front();
		SEList.pop();
		MicroNode *to_node = &micro_node_vector[to_node_seq_no];

		for (int i = 0; i < to_node->m_incoming_link_vector.size(); i++)
		{
			MicroLink *incoming_link = &micro_link_vector[micro_link_id_to_seq_no_dict[to_node->m_incoming_link_vector[i]]];
			MicroNode *from_node = &micro_node_vector[micro_node_id_to_seq_no_dict[incoming_link->from_node_id]];
			if (from_node->meso_link_id != mesolink->link_id)
				continue;

			float new_from_node_cost = micro_node_label_cost[to_node->node_seq_no] + incoming_link->free_flow_travel_time_in_simu_interval + incoming_link->additional_cost;

			if (new_from_node_cost < micro_node_label_cost[from_node->node_seq_no])
			{
				micro_node_label_cost[from_node->node_seq_no] = new_from_node_cost;
				micro_node_predecessor[from_node->node_seq_no] = to_node->node_seq_no;
				micro_link_predecessor[from_node->node_seq_no] = incoming_link->link_seq_no;
				SEList.push(from_node->node_seq_no);

			}
		}
	}

	delete micro_node_predecessor;
	delete micro_link_predecessor;
	return micro_node_label_cost;
}



// ==== Begin: Debug print and validation for backward tree ====
void printBackwardTree(int movement_id, MesoLink* p_link) {
	debug_log_file << "\n[DEBUG] Backward Tree for movement " << movement_id
		<< " on meso link " << p_link->link_id << ":\n";

	auto& cost_tree = p_link->estimated_cost_tree_for_each_movement[movement_id];
	for (std::map<int, float>::iterator it = cost_tree.begin(); it != cost_tree.end(); ++it) {
		debug_log_file << "NodeSeqNo: " << it->first << ", Cost: " << it->second << "\n";
	}
}


void validateBackwardTree(int movement_id, MesoLink* p_link) {
	auto& cost_tree = p_link->estimated_cost_tree_for_each_movement[movement_id];
	bool has_valid_path = false;
	for (std::map<int, float>::iterator it = cost_tree.begin(); it != cost_tree.end(); ++it) {
		if (it->second < _MAX_COST_TREE) {
			has_valid_path = true;
			break;
		}
	}
	if (!has_valid_path) {
		std::cerr << "[ERROR] No valid path found in backward tree for movement "
			<< movement_id << " on meso link " << p_link->link_id << std::endl;
	}
}


void dumpBackwardTreeToTXT(int movement_id, MesoLink* p_link) {
	std::string filename = "output/debug/tree_" + std::to_string(p_link->link_id) + "_" +
		std::to_string(movement_id) + ".txt";
	std::ofstream file(filename.c_str());

	file << "NodeSeqNo,Cost\n";
	auto& cost_tree = p_link->estimated_cost_tree_for_each_movement[movement_id];
	for (std::map<int, float>::iterator it = cost_tree.begin(); it != cost_tree.end(); ++it) {
		file << it->first << "," << it->second << "\n";
	}
	file.close();
}

void dumpTurningMIDNodesToTXT(int movement_id, MesoLink* p_link, Network* net) {
	std::string filename = "output/debug/mid_nodes_link" + std::to_string(p_link->link_id) +
		"_move" + std::to_string(movement_id) + ".txt";

	std::ofstream file(filename.c_str());
	file << "NodeID,NodeSeqNo,X,Y\n";

	const std::vector<int>& node_seq_nos = p_link->turning_node_seq_no_dict[movement_id];
	for (size_t i = 0; i < node_seq_nos.size(); ++i) {
		int seq_no = node_seq_nos[i];
		const MicroNode& node = net->micro_node_vector[seq_no];
		file << node.node_id << "," << seq_no << "," << node.x << "," << node.y << "\n";
	}

	file.close();
}

// ==== End: Debug print and validation for backward tree ====
