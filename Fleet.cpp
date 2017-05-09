#include <iostream>
#include <thread>
#include <mutex>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <atomic>
#include <algorithm>
#include "Fleet.h"

using namespace std;

void simulateAttacks(Fleet* fleet, Gaia *gaia, int num);
void coloniseGaia(Fleet* fleet1, Fleet* fleet2, Gaia *gaia, unsigned int num);
void sort_Velocity(vector<int>& vel, vector<Fleet*>& fleet);

mutex mtx;           // mutex for critical section

//static long long velocity = 0;

bool checkVelocity = false;

vector<thread> threads;
vector<long long> velocityCheck;
vector<Fleet*> fleet_list;

Fleet *fleet1;
Fleet *fleet2;
Gaia *gaia = new Gaia();

/*****************Fleet Class Implementation*************************/
Fleet::Fleet(string corpName){
    corporationName = corpName;
}

string Fleet::getCorporationName() const{
    return corporationName;
}

void Fleet::setMedicStatus(bool flag){
    medicStatus = flag;
}

bool Fleet::hasMedic() const{
    return medicStatus;
}

void Fleet::addShip(Ship* obj){
    list_Ship.push_back(obj);
}

void Fleet::add_ColonyShip(Ship* obj){
    list_ColonyShip.push_back((ColonyShip*)obj);
}

void Fleet::add_MilitaryShip(MilitaryEscortShip* obj){
    list_MilitaryShip.push_back(obj);
}

void Fleet::add_SolarShip(SolarSailShip* obj){
    list_SolarShip.push_back(obj);
}

vector<Ship*> Fleet::shipList() const{
    return list_Ship;
}

vector<Ship*> Fleet::colonyShips() const{
    vector<Ship*> cShip_list;

    for(unsigned int i=0; i < list_ColonyShip.size(); i++){
        cShip_list.push_back(list_ColonyShip[i]);
    }
    return cShip_list;
}

void Fleet::shipDisplay(){
    for(unsigned int i = 0; i < list_Ship.size(); i++){
        cout << list_Ship[i]->getTypeName() << endl;
    }
}

void Fleet::destroyShip(Ship* i){

    for(auto itr = list_ColonyShip.begin(); itr != list_ColonyShip.end(); itr++){
        if(*itr == i){
            list_ColonyShip.erase(itr);
            return;
        }
    }
}

int Fleet::getColonistCount() const{
    int colonistCount = 0;

    for(unsigned int i = 0; i < list_ColonyShip.size(); i++){
        colonistCount += list_ColonyShip[i]->getColonistCount();
    }

    return colonistCount;
}

void Fleet::setCost(int cost){
    costOfFleet = cost;
}

int Fleet::getCost() const{
    return costOfFleet;
}

int Fleet::EnergyProduction() const{
    int totalEnergyProduction = 0;
    for(unsigned int i = 0; i < list_SolarShip.size(); i++){
        totalEnergyProduction += list_SolarShip[i]->getEnergyProduction();
    }

    return totalEnergyProduction;
}

void Fleet::setEnergyConsumption(int energy){
    total_energyConsumption = energy;
}

int Fleet::getEnergyConsumption() const{
    return total_energyConsumption;
}

void Fleet::setWeight(int weight){
    fleetWeight = weight;
}

int Fleet::getWeight() const{
    return fleetWeight;
}

long long Fleet::getVelocity() const{
     //long secondsInAYear = 31557600;
    long long speedOfLight = 299792458;
    long long velocity = (10*speedOfLight)/sqrt(getWeight());
    long long speed = velocity*31557600;//multiply with total seconds in a year

    return speed;//meter per year
}

void Fleet::setDistanceToGaia(){
    long long lightYear_In_Meters = 9460730472580800;
    distance_To_Gaia = lightYear_In_Meters*33;
}

long long Fleet::getDistanceToGaia(){
    setDistanceToGaia();
    return distance_To_Gaia;
}

int Fleet::getYearsToGaia(){
    long long x = getDistanceToGaia();
    int number = 0;
    number = x / this->getVelocity();

    years_to_Gaia = number;
    return years_to_Gaia;
}

int Fleet::countProtectedShips() const{
    int totalProtected = 0;
    for(unsigned int i = 0; i < list_MilitaryShip.size(); i++){
        totalProtected += list_MilitaryShip[i]->getNrProtected();
    }

    return totalProtected;
}

vector<Ship*> Fleet::protectedShips() const{
    vector<Ship*> colony_ship = colonyShips();
    unsigned int protectedShips = countProtectedShips();

    if(protectedShips >= colony_ship.size()){
        return colony_ship;
    }

    return vector<Ship*>(colony_ship.begin(),colony_ship.begin()+protectedShips);
}

vector<Ship*> Fleet::unprotectedShips() const{
    vector<Ship*> colony_ship = colonyShips();
    unsigned int protectedShips = countProtectedShips();

    if(protectedShips >= colony_ship.size()){
        return vector<Ship*>();
    }

    return vector<Ship*>(colony_ship.begin()+protectedShips, colony_ship.end());
}

void Fleet::displayProtectShip(){
    vector<Ship*> pShip = protectedShips();
    for(unsigned i = 0; i < pShip.size(); i++){
        cout << pShip[i]->getTypeName() << endl;
    }
}

void Fleet::display_unProtectShip(){
    vector<Ship*> up_Ship = unprotectedShips();
    if(up_Ship.empty()){
        cout << "None" << endl;
    }
    else{
        for(unsigned i = 0; i < up_Ship.size(); i++){
            cout << up_Ship[i]->getTypeName() << endl;
        }
    }
}

void Fleet::spaceAttack(){
    vector<Ship*> up_Ship = unprotectedShips();
    if(up_Ship.empty()){
        cout << "All ships are protected by military." << endl;
    }
    else{
        int shipsToBeDestroyed = ceil(0.25*up_Ship.size());
        cout << "Total ship(s) destroyed by alien space attack: " << shipsToBeDestroyed << endl;
        for(int i = 0; i < shipsToBeDestroyed; i++){
           int randomShip = rand() % up_Ship.size();
           cout << "Type of colonist ship(s) destroyed: " << endl <<up_Ship[randomShip]->getTypeName() << endl;
           destroyShip(up_Ship[randomShip]);
        }
    }
}

void Fleet::diseaseOutBreak(){
    vector<ColonyShip*> cShip = list_ColonyShip;
    if(cShip.empty()){
        cout << "No more colonist ships." << endl;
    }
    else if(hasMedic()){
        cout << "This fleet has medic. Disease has been contained and neutralised." << endl;
    }
    else if(!hasMedic()){
        int randomShip = rand() % cShip.size();
        cout << "All the colonist on " << cShip[randomShip]->getTypeName() << " has died." << endl;
        cShip[randomShip]->infect();
    }
}

/*****************Ship Class Implementation*************************/
Ship ::Ship(string shipType){
    nameType = shipType;
}

string Ship::getTypeName() const{
    return nameType;
}

/*****************Colony Ship Class Implementation*************************/
ColonyShip::ColonyShip(string shipType):Ship(shipType){}

void ColonyShip::setColonistCount(int num){
    colonistCount = num;
}

int ColonyShip::getColonistCount() const{
    return  colonistCount;
}

void ColonyShip::infect(){
    colonistCount = 0;
}

bool ColonyShip::isInfected() const{
    return colonistCount == 0;
}

/*****************Military Ship Class Implementation*************************/
MilitaryEscortShip::MilitaryEscortShip(string shipType):Ship(shipType){}

void MilitaryEscortShip::setFighters(int num){
    numOfFighters = num;
}

int MilitaryEscortShip::getNrFighters() const{
    return numOfFighters;
}

int MilitaryEscortShip::getNrProtected() const{
    return (getNrFighters()/2) + 1;
}

/*****************Solar Ship Class Implementation*************************/
SolarSailShip::SolarSailShip(string shipType):Ship(shipType){}

void SolarSailShip::setEnergyProduction(int energy){
    energyProduction = energy;
}

int SolarSailShip::getEnergyProduction() const{
    return energyProduction;
}

/*****************Planet Gaia Implementation*************************/
Gaia::Gaia(){}

void Gaia::setPopulation(int colonistNum){
    population = colonistNum;
}

int Gaia::getPopulation() const{
    return population;
}

void Gaia::increasePopulation(){
    population *= 1.05;
}

void Gaia::setColonyName(string name){
    colonyName = name;
}

string Gaia::getColonyName() const{
    return colonyName;
}


/***************************START SIMULATION******************************/
void startSimulation (int thread_ID, string data) {
    // critical section
    mtx.lock();

    int totalCost = 0;
    int totalWeight = 0;
    int totalEnergyConsumption = 0;


    ifstream dataFile;
    dataFile.open(data);

    char shipType[30];
    int numberOfShips;
    int counter_i = 0;
    dataFile >> shipType;

    Fleet *fleet = new Fleet(shipType);
    //cout << fleet->getCorporationName() << endl;
    cout << "\n\n*************************************************NEW SIMULATION*******************************************************" << endl;
    while(!dataFile.eof()){

        dataFile >> shipType;//read type of ship

        //colony ships
        if(strcmp(shipType,"Ferry") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                ColonyShip *Ferry = new ColonyShip("Ferry");
                Ferry->setColonistCount(100);
                //cout << Ferry->getTypeName() << "_" << counter_i+1 << " " << Ferry->getColonistCount() << endl;
                fleet->addShip(Ferry);
                fleet->add_ColonyShip(Ferry);

                //add cost, weight and energy consumption to the fleet
                totalCost += 500;
                totalWeight += 10;
                totalEnergyConsumption += 5;
            }

        }
        else if(strcmp(shipType,"Liner") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                ColonyShip *Liner = new ColonyShip("Liner");
                Liner->setColonistCount(250);
                //cout << Liner->getTypeName() << "_" << counter_i+1 << " " << Liner->getColonistCount()<< endl;
                fleet->addShip(Liner);
                fleet->add_ColonyShip(Liner);

                //add cost, weight and energy consumption to the fleet
                totalCost += 1000;
                totalWeight += 20;
                totalEnergyConsumption += 7;
            }
        }
        else if(strcmp(shipType,"Cloud") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                ColonyShip *Cloud = new ColonyShip("Cloud");
                Cloud->setColonistCount(750);
                //cout << Cloud->getTypeName() << " " << counter_i << endl;
                fleet->addShip(Cloud);
                fleet->add_ColonyShip(Cloud);

                //add cost, weight and energy consumption to the fleet
                totalCost += 2000;
                totalWeight += 30;
                totalEnergyConsumption += 9;
            }
        }

        //solar sail ships
        else if(strcmp(shipType,"Radiant") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                SolarSailShip *Radiant = new SolarSailShip("Radiant");
                //cout << Radiant->getTypeName() << " " << counter_i << endl;
                Radiant->setEnergyProduction(50);
                fleet->addShip(Radiant);
                fleet->add_SolarShip(Radiant);


                //add cost, weight and energy consumption to the fleet
                totalCost += 50;
                totalWeight += 3;
                totalEnergyConsumption += 5;
            }
        }
        else if(strcmp(shipType,"Ebulient") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                SolarSailShip *Ebulient = new SolarSailShip("Ebulient");
                //cout << Ebulient->getTypeName() << " " << counter_i << endl;
                Ebulient->setEnergyProduction(500);
                fleet->addShip(Ebulient);
                fleet->add_SolarShip(Ebulient);

                //add cost, weight and energy consumption to the fleet
                totalCost += 250;
                totalWeight += 50;
                totalEnergyConsumption += 5;
            }
        }

        //military escort ship
        else if(strcmp(shipType,"Cruiser") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                MilitaryEscortShip *Cruiser = new MilitaryEscortShip("Cruiser");
                Cruiser->setFighters(0);
                //cout << Cruiser->getTypeName() << " " << counter_i << endl;
                fleet->addShip(Cruiser);
                fleet->add_MilitaryShip(Cruiser);

                //add cost, weight and energy consumption to the fleet
                totalCost += 300;
                totalWeight += 2;
                totalEnergyConsumption += 10;
            }

        }
        else if(strcmp(shipType,"Frigate") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                MilitaryEscortShip *Frigate = new MilitaryEscortShip("Frigate");
                Frigate->setFighters(10);
                //cout << Frigate->getTypeName() << " " << counter_i << endl;
                fleet->addShip(Frigate);
                fleet->add_MilitaryShip(Frigate);

                //add cost, weight and energy consumption to the fleet
                totalCost += 1000;
                totalWeight += 7;
                totalEnergyConsumption += 20;
            }
        }
        else if(strcmp(shipType,"Destroyer") == 0){
            dataFile >> numberOfShips;

            for(counter_i = 0; counter_i < numberOfShips; counter_i++){
                MilitaryEscortShip *Destroyer = new MilitaryEscortShip("Destroyer");
                Destroyer->setFighters(25);
                //cout << Destroyer->getTypeName() << " " << counter_i << endl;
                fleet->addShip(Destroyer);
                fleet->add_MilitaryShip(Destroyer);

                //add cost, weight and energy consumption to the fleet
                totalCost += 2000;
                totalWeight += 19;
                totalEnergyConsumption += 30;
            }
        }

        //medic ship
        else if(strcmp(shipType,"Medic") == 0){
            dataFile >> numberOfShips;
            fleet->setMedicStatus(true);
            //cout << "Status of Medic: " << fleet->hasMedic() << endl;

            //add cost, weight and energy consumption to the fleet
            totalCost += 1000;
            totalWeight += 1;
            totalEnergyConsumption += 1;
        }

        //if there is a wrong ship input
        else{
            cout << "Please check your DAT file. You have entered a wrong ship." << endl;
            cout << "Wrong ship: " << shipType << "."<< endl;
            break;
        }

    }

    dataFile.close();
    cout << endl;

    cout << "Fleet " << fleet->getCorporationName() << " has the following ships in its fleet:- " << endl;
    fleet->shipDisplay();

    if(fleet->hasMedic()){
        cout << "Medic" << endl;
    }

    cout << "\nTotal cost to build fleet: " << totalCost << endl;


    //validate cost of fleet
    fleet->setCost(totalCost);

    if(totalCost > 10000){
        cout << "\nTotal cost (UNP) to build fleet exceeded by: " << (10000-totalCost) << endl;
        cout << "Please build a new fleet under 10000 UNP.\n" << endl;
        exit (EXIT_FAILURE);
    }

    //total colonist in the fleet
    cout << "Total colonist in the fleet: " << fleet->getColonistCount() << endl;

    //validate energy consumption & production of the fleet

    int totalEnergyProduction = fleet->EnergyProduction();
    fleet->setEnergyConsumption(totalEnergyConsumption);

    cout << "Energy Production: " << totalEnergyProduction << endl;
    cout << "Total energy consumption: " << fleet->getEnergyConsumption() << endl;



    if(totalEnergyConsumption > totalEnergyProduction){
        cout << "\nTotal cost energy consumption exceeded by: " << (totalEnergyConsumption - totalEnergyProduction) << endl;
        cout << "Please build a new fleet with less ships or more solar sail ships.\n" << endl;
        exit (EXIT_FAILURE);
    }

    fleet->setWeight(totalWeight);
    cout << "Total weight of the fleet: " << fleet->getWeight() << endl;
    cout << "Velocity of the fleet: " << fleet->getVelocity() << " m/year." << endl;

    cout << "Distance to Gaia: " << fleet->getDistanceToGaia() << " meters." << endl;
    cout << "Years taken to travel to Gaia: " << fleet->getYearsToGaia() << " years." << endl;

    cout << "\nNumber of ships protected: " << fleet->countProtectedShips() << endl;

    cout << "\nProtected ships: " << endl;
    fleet->displayProtectShip();

    cout << "\nUnprotected ships: " << endl;
    fleet->display_unProtectShip();
    cout << endl;


    velocityCheck.push_back(fleet->getVelocity());
    fleet_list.push_back(fleet);

    mtx.unlock();
    cout << endl;

}

void simulateAttacks(Fleet* fleet, Gaia *gaia, int num){

    lock_guard<mutex> guard(mtx);
    /***********************SPACE ATTACKS BY ALIENS**********************/

    cout << "*********************************************SPACE ATTACKS BY ALIENS*************************************************" << endl;
    cout << "\nAt year " << (fleet->getYearsToGaia()/2) << ",Fleet " << fleet->getCorporationName() << " was attacked. The report as follows: "<< endl;
    fleet->spaceAttack();
    cout << "Total colonist in the fleet after alien attack: " << fleet->getColonistCount() << endl;
    cout << "\n"<<endl;

    /*****************************DISEASE OUTBREAK*****************************/


    cout << "***********************************************DISEASE OUTBREAK*******************************************************" << endl;
    cout << "\nShortly after space attack,Fleet " << fleet->getCorporationName() << " was hit with a disease outbreak in one of its ships. The report as follows: "<< endl;
    fleet->diseaseOutBreak();
    cout << "Total colonist in the fleet after disease outbreak: " << fleet->getColonistCount() << endl;
    cout << "\n"<<endl;
}

void coloniseGaia(Fleet* fleet1, Fleet* fleet2, Gaia *gaia,  unsigned int num){

    lock_guard<mutex> guard(mtx);
    cout << "\n\n******************************************COLONISATION BEGINS******************************************************" << endl;
    cout << "\n\nFleet " << fleet1->getCorporationName() << " is entering Gaia." << endl;

    if(gaia->getPopulation() == 0){
        cout << "Checking population...\nPlanet Gaia's is uncolonised." << endl;
        cout << "Fleet " << fleet1->getCorporationName() << " has colonised Gaia." << endl;

        gaia->setPopulation(fleet1->getColonistCount());
        gaia->setColonyName(fleet1->getCorporationName());

        int yearsLeft = fleet1->getYearsToGaia() - fleet2->getYearsToGaia();
        cout << "Years left before another fleet reaches: " << abs(yearsLeft) << endl;

        for(int i = 1; i <= abs(yearsLeft); i++){ gaia->increasePopulation();}

        cout << "Gaia's current population after " << abs(yearsLeft) << " years: " << gaia->getPopulation() << "\n\n"<<endl;
    }

    else if(gaia->getPopulation() != 0 && num != fleet_list.size() ){
        cout << "Checking population...\nPlanet has been colonised. " << endl;
        if(fleet1->getColonistCount() < gaia->getPopulation()){
            cout << "Current Gaia population: " << gaia->getPopulation();
            cout << "   Current Fleet population: " << fleet1->getColonistCount() << endl;
            cout << "Planet Gaia's population is higher. Searching for another planet..." << endl;

            int yearsLeft = fleet1->getYearsToGaia() - fleet2->getYearsToGaia();
            cout << "\n\nYears left before another fleet reaches: " << abs(yearsLeft) << endl;

            for(int i = 1; i <= abs(yearsLeft); i++){ gaia->increasePopulation();}
            cout << "\n\nGaia's current population after " << abs(yearsLeft) << " years: " << gaia->getPopulation() << "\n\n"<<endl;


        }
        else if(fleet1->getColonistCount() > gaia->getPopulation()){
            cout << "Current Gaia population: " << gaia->getPopulation();
            cout << "   Current Fleet population: " << fleet1->getColonistCount() << endl;
            cout << "Planet Gaia's population is lower. This fleet can taken over Gaia." << endl;
            cout << "Colonising planet..." << endl;
            gaia->setPopulation(fleet1->getColonistCount());
            gaia->setColonyName(fleet1->getCorporationName());
            cout << "New population count: " << gaia->getPopulation() << endl;

            int yearsLeft = fleet1->getYearsToGaia() - fleet2->getYearsToGaia();
            cout << "Years left before another fleet reaches: " << abs(yearsLeft) << endl;

            for(int i = 1; i <= abs(yearsLeft); i++){ gaia->increasePopulation();}

            cout << "Gaia's current population after " << abs(yearsLeft) << " years: " << gaia->getPopulation() << "\n\n"<<endl;
        }
    }
    else if(gaia->getPopulation() != 0 && num == fleet_list.size() ){
        cout << "Checking population...\nPlanet has been colonised. " << endl;
        if(fleet1->getColonistCount() < gaia->getPopulation()){
            cout << "Current Gaia population: " << gaia->getPopulation();
            cout << "   Current Fleet population: " << fleet1->getColonistCount() << endl;
            cout << "Planet Gaia's population is higher. Searching for another planet..." << endl;

        }
        else if(fleet1->getColonistCount() > gaia->getPopulation()){
            cout << "Current Gaia population: " << gaia->getPopulation();
            cout << "   Current Fleet population: " << fleet1->getColonistCount() << endl;
            cout << "Planet Gaia's population is lower. This fleet can taken over Gaia." << endl;
            cout << "Colonising planet..." << endl;
            gaia->setPopulation(fleet1->getColonistCount());
            gaia->setColonyName(fleet1->getCorporationName());
            cout << "New population count: " << gaia->getPopulation() << endl;
        }
        cout << "\n\n\nNO MORE FLEETS INCOMING..." << endl;
    }
}

void sort_Velocity(vector<long long>& vel, vector<Fleet*>& fleet){
    Fleet* temp_fleet;
    bool swapp = true;
    while(swapp){
        swapp = false;
        for (size_t i = 0; i < vel.size()-1; i++) {
            if (vel[i] < vel[i+1] ){
                // swap velocity
                vel[i] += vel[i+1];
                vel[i+1] = vel[i] - vel[i+1];
                vel[i] -=  vel[i+1];

                // swap fleet pointer
                temp_fleet = fleet[i];
                fleet[i] = fleet[i+1];
                fleet[i+1] = temp_fleet;


                swapp = true;
            }
        }
    }
}

int main ()
{

    threads.push_back(thread(startSimulation,1,"Fleet3.dat"));
    threads.push_back(thread(startSimulation,2,"Fleet1.dat"));
    threads.push_back(thread(startSimulation,3,"Fleet2.dat"));
    threads.push_back(thread(startSimulation,4,"Fleet4.dat"));

    for (auto& th : threads) th.join();
    threads.clear();

    sort_Velocity(velocityCheck,fleet_list);

    cout << "All the fleets has departed....\n\n\n" << endl;

    //space attack & disease outbreak
    for(unsigned int i = 0; i < fleet_list.size(); i++){
        threads.push_back(thread(simulateAttacks,fleet_list[i], gaia, i));
    }

    for (auto& th : threads) th.join();
    threads.clear();

    //colonise planet
    for(unsigned int i = 0; i < fleet_list.size(); i++){
        threads.push_back(thread(coloniseGaia,fleet_list[i], fleet_list[i+1],gaia , i+1));
    }

    for (auto& th : threads) th.join();


    cout << "\n\nWINNER: FLEET " << gaia->getColonyName() << endl;

    threads.clear();

  return 0;
}


