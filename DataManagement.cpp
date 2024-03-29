#include "DataManagement.h"

using namespace std;


void DataManager::addStudent(Student student) {
	
	students.push_back(student);
}

void DataManager::addSchool(School school) {

	schools.push_back(school);
}
void DataManager::addBusStop(BusStop busStop)
{
	busStops.push_back(busStop);
}
void DataManager::addAllBusStop(BusStop busStop)
{
	allBusStops.push_back(busStop);
}

int DataManager::readNodeFile(string fileName)
{
	ifstream nodeFile;
	string nodeInfo;
	int nodeId;
	float nodeX, nodeY;

	nodeFile.open(fileName);
	if (!nodeFile.is_open())
	{
		cerr << "Input file not found, try again!" << endl;
		return 1;
	}

	getline(nodeFile, nodeInfo);

	while (!nodeFile.eof()) {
		getline(nodeFile, nodeInfo);
		nodeId = stoi(nodeInfo.substr(1, nodeInfo.find(',') - 1));
		nodeInfo.erase(0, nodeInfo.find(',') + 2);
		nodeX = stof(nodeInfo.substr(0, nodeInfo.find(',')));
		nodeInfo.erase(0, nodeInfo.find(',') + 2);
		nodeY = stof(nodeInfo.substr(0, nodeInfo.find(')')));

		Node node = Node(nodeId, nodeX, nodeY);
		graph.addVertex(node);
	}

	cout<<endl;
	cout << endl << "--------------------" << endl;
	cout << "Nodes Load Finished!" << endl;
	cout << "--------------------" << endl << endl;
	return 0;
}


Node getNode(int id, Graph<Node> &graph) {
	Node node(-1,-1,-1);
	for (auto v : graph.getVertexSet()) {
		if (v->getInfo().getID() == id)
			return (v->getInfo());
	}

	return node;
}

int DataManager::readEdgeFile(string fileName) {
	ifstream edgeFile;
	int edgeSrc, edgeDst;
	edgeFile.open(fileName);
	if (!edgeFile.is_open())
	{
		cerr << "Input file not found, try again!" << endl;
		return 1;
	}

	string edgeInfo;
	getline(edgeFile, edgeInfo);

	while (!edgeFile.eof()) {
		getline(edgeFile, edgeInfo);
		edgeSrc = stoi(edgeInfo.substr(1, edgeInfo.find(',') - 1));
		edgeInfo.erase(0, edgeInfo.find(',') + 2);
		edgeDst = stoi(edgeInfo.substr(0, edgeInfo.find(')')));
		Node scr = getNode(edgeSrc, graph);
		Node dst = getNode(edgeDst, graph);
		graph.addEdge(scr, dst, scr.getDistance(dst));
		graph.addEdge(dst, scr, scr.getDistance(dst));
	}

	cout<<endl;
	cout << endl << "--------------------" << endl;
	cout << "Edges Load Finished!" << endl;
	cout << "--------------------" << endl << endl;
	return 0;
}

int DataManager::loadPointsOfInterest(string fileName) {
	ifstream poiFile;
	poiFile.open(fileName);
	string line, sName, BSaddress;
	int nodeID, morningTime, afternoonTime;

	if (!poiFile.is_open())
	{
		cerr << "Input file not found, try again!" << endl;
		return 1;
	}

	getline(poiFile, line);

	while (!poiFile.eof())
	{
		getline(poiFile, line);

		if (line == "house")
		{
			break;
		}
		nodeID = stoi(line.substr(0, line.find(',')));
		line.erase(0, line.find(',') + 2);
		sName = line.substr(0, line.find(','));
		line.erase(0, line.find(',') + 2);
		morningTime = stoi(line.substr(0, line.find(',')));
		line.erase(0, line.find(',') + 2);
		afternoonTime = stoi(line.substr(0, line.find(',')));
		line.erase(0, line.find(',') + 2);


		for(unsigned int i=0;i<graph.getVertexSet().size(); i++)
		{
			if(graph.getVertexSet().at(i)->getInfo().getID() == nodeID)
			{
				School newSchool = School(sName, morningTime, afternoonTime, graph.getVertexSet().at(i));
				addSchool(newSchool);
				break;
			}
		}

	}

	while (!poiFile.eof())
	{

		getline(poiFile, line);
		if (line == "garage")
		{
			break;
		}
		nodeID = stoi(line.substr(0, line.find(',')));
		line.erase(0, line.find(',') + 2);
		BSaddress = line.substr(0, line.find(','));

		for(unsigned int i=0;i<graph.getVertexSet().size(); i++)
		{
			if(graph.getVertexSet().at(i)->getInfo().getID() == nodeID)
			{
				BusStop newBS = BusStop(graph.getVertexSet().at(i), BSaddress);
				addAllBusStop(newBS);
				break;
			}

		}

	}

	getline(poiFile,line);
	for(unsigned int i=0;i<graph.getVertexSet().size(); i++)
	{
		if(graph.getVertexSet().at(i)->getInfo().getID() == stoi(line))
		{
			vector<Bus> buses;
			Garage garage(buses, 0, graph.getVertexSet().at(i));
			setGarage(garage);
			break;
		}
	}

	cout<<endl;
	cout << endl << "--------------------" << endl;
	cout << "Tags Load Finished!" << endl;
	cout << "--------------------" << endl << endl;
	cout<<endl;
	return 0;

}

void DataManager::organizeBuses()
{
	vector<Bus> vec;
	int max = 0, ind;

	while(!garage.getBuses().empty()){
		max = 0;
		ind =1000;
		for(unsigned int i = 0; i < garage.getBuses().size(); i++){
				if(garage.getBuses().at(i).getCapacity() >max){
					max = garage.getBuses().at(i).getCapacity();
					ind = i;
				}
			}

			vec.push_back(garage.getBuses().at(ind));
			garage.eraseBus(ind);
	}

	for(unsigned int j = 0; j < vec.size(); j++){
		garage.addBus(vec.at(j));
	}

}

void DataManager::increaseStudent(Student* student, int index)
{
	busStops.at(index).addStudentInStop(student);
}

void DataManager::updateQ(vector<Node> newBusStop)
{

	while (!nearestV.empty())
	{
		nearestV.extractMin();
	}


	for (unsigned int i = 0; i < newBusStop.size(); i++)
	{
		Vertex<Node> *v = graph.findVertex(newBusStop.at(i));
		nearestV.insert(v);
	}
}

void DataManager::eraseNodeWithID(vector<Node>& bStops, int id){
	for(unsigned int i = 0; i < bStops.size(); i++){
		if(bStops.at(i).getID() == id){
			bStops.erase(bStops.begin()+i);
			return;
		}
	}
}

vector<Node> DataManager::toVecNode(vector<BusStop> b)
{
	vector<Node> temp;
	for (unsigned int i = 0; i < b.size(); i++)
	{
		temp.push_back(b.at(i).getNode()->getInfo());
	}

	return temp;
}

BusStop *DataManager::getBusStopByNode(vector<BusStop> &temp, Vertex<Node> *node)
{
	for(unsigned int i=0; i< temp.size(); i++)
	{
		if(temp.at(i).getNode()->getInfo().getID() == node->getInfo().getID())
		{

			return &temp.at(i);
		}
	}

	return NULL;
}

bool DataManager::verifyNumberStudents()
{
	int busTotalCapacity = 0;
	int numStudents = 0;

	for(unsigned int i=0; i< garage.getBuses().size(); i++)
	{
		busTotalCapacity += garage.getBuses().at(i).getCapacity();
	}

	for (unsigned int i = 0; i < busStops.size(); i++)
	{
		numStudents += busStops.at(i).getStudentsInStop()->size();
	}

	return numStudents > busTotalCapacity;
}

int getIndexFromTemp(vector<BusStop> temp, int id)
{
	for(unsigned int i=0; i< temp.size(); i++)
	{
		if(temp.at(i).getNode()->getInfo().getID() == id)
		{
			return i;
		}
	}
	return -1;
}

vector<vector<Node>> DataManager::getMultPaths(){//Return vector of vectors with ordered BusStops 1 per each bus

	vector<vector<Node>> bsVec;
	if(busStops.size() == 0)
	{
		cout << "There aren't bus stops"<<endl;
		return bsVec;
	}

	if(garage.getBuses().size() == 0)
	{
		return bsVec;
	}

	if (verifyNumberStudents())
	{
		return bsVec;
	}

	vector<vector<Node>> allBusesPaths;
	vector<Node> busXpath, temp2;
	vector<BusStop> temp;
	temp = busStops;
	int i=0;
	while (!temp.empty())
	{
		graph.dijkstraShortestPath(garage.getNode()->getInfo());
		temp2 = toVecNode(temp);
		updateQ(temp2);
		int capacity = garage.getBuses().at(i).getCapacity();
		int occupation = garage.getBuses().at(i).getOccupation();

		while (occupation != capacity && !temp.empty())
		{

			Vertex<Node>* node = nearestV.extractMin();
			BusStop* b = getBusStopByNode(temp, node); 

			int numS = b->getStudentsInStop()->size();

			if(node->getDist()==INF)
			{
				cout<<"There is no path"<<endl;
				//return bsVec;
			}

			if ((numS + occupation) > capacity)
			{
				int numStudents = capacity - occupation;
				for (unsigned int k = 0; k < numStudents; k++)
				{
					b->eraseStudentInStop(k);
				}
				occupation += numStudents;
				nearestV.insert(node);

				busXpath.push_back(node->getInfo());

			}
			else
			{
				occupation += b->getStudentsInStop()->size();
				//cout<<"NODE ID para o vec"<< node->getInfo().getID()<<endl;
				busXpath.push_back(node->getInfo());
				int j = getIndexFromTemp(temp, b->getNode()->getInfo().getID());
				temp.erase(temp.begin()+j);
				graph.dijkstraShortestPath(garage.getNode()->getInfo());
				temp2 = toVecNode(temp);
				updateQ(temp2);

			}

		}

		i++;
		vector<Node> path;
		path = getPathNodes(busXpath);
		bsVec.push_back(path);

		busXpath.clear();

	}



	return bsVec;
}

vector<Node> DataManager::getPathNodes(vector<Node> &bStops) //Returns vector with ordered BusStops
{
	Vertex<Node> nearV(*new Node(-1,-1,-1));
	vector<Node> path;

    graph.dijkstraShortestPath(garage.getNode()->getInfo());

	updateQ(bStops);

	nearV = *nearestV.extractMin();
	path.push_back(garage.getNode()->getInfo());
	path.push_back(nearV.getInfo());

	unsigned int BSsize = bStops.size() + 1;

	while(path.size() != BSsize){

		eraseNodeWithID(bStops, nearV.getInfo().getID());
		graph.dijkstraShortestPath(nearV.getInfo());
		updateQ(bStops);
		nearV = *nearestV.extractMin();
		path.push_back(nearV.getInfo());
	}

	path.push_back(school.getNode()->getInfo());

	return path;

}

vector<vector<Node>> DataManager::getPath() //Returns vector assembled Paths
{
	vector<vector<Node>> pathNode;
	vector<vector<Node>> finalPath;
	pathNode = getMultPaths();

	for(unsigned int k=0; k<pathNode.size(); k++)
	{
		for(unsigned int i = 0 ; i < pathNode.at(k).size(); i++){
		}
		vector<Node> temp, nodePath;
		nodePath.push_back(garage.getNode()->getInfo());

		for(unsigned int j = 0; j < pathNode.at(k).size() -1; j++)
		{
			graph.dijkstraShortestPath(pathNode.at(k).at(j));
			temp = graph.getPath(pathNode.at(k).at(j), pathNode.at(k).at(j+1));

			for(unsigned int i = 1;i < temp.size(); i++){
				nodePath.push_back(temp.at(i));
			}
		
		}
		finalPath.push_back(nodePath);
	}

	return finalPath;

}

