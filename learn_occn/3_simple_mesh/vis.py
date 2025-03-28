import json
import networkx as nx
import matplotlib.pyplot as plt

def visualize_noc(json_file):
    with open(json_file, 'r') as f:
        noc_data = json.load(f)
    
    G = nx.DiGraph()  # Directed graph (since channels are bidirectional)
    
    # Add nodes
    for node_name, node_info in noc_data["nodes"].items():
        G.add_node(node_name, pos=(node_info["x"], node_info["y"]))
    
    # Add edges (connections)
    for conn_name, conn_info in noc_data["connections"].items():
        G.add_edge(conn_info["from"], conn_info["to"], direction=conn_info["direction"])
    
    # Draw the graph
    pos = nx.get_node_attributes(G, 'pos')
    nx.draw(G, pos, with_labels=True, node_size=1000, node_color='lightblue', font_size=10, font_weight='bold')
    
    # Draw edge labels (directions)
    edge_labels = nx.get_edge_attributes(G, 'direction')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
    
    plt.title("2x2 Mesh NoC Topology")
    plt.show()

if __name__ == "__main__":
    visualize_noc("noc_topology.json")
