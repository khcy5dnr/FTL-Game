#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*Class Definitions*/
class Ship{
public:

    Ship(string shipType);
    ~Ship();

    string nameType;
    int numberOfShips;

    int getEnergyConsumption() const;
    int getWeight() const;
    int getCost() const;
    int getShipCount();
    string getTypeName() const;
    bool isDestroyed() const;
};

class ColonyShip: public Ship{
private:
    int colonistCount = 0;
public:
    ColonyShip(string shipType);
    ~ColonyShip();

    void setColonistCount(int);
    int getColonistCount() const;
    void infect();
    bool isInfected() const;

};

class SolarSailShip: public Ship{
private:
    int energyProduction = 0;
public:
    SolarSailShip(string shipType);
    ~SolarSailShip();

    void setEnergyProduction(int);
    int getEnergyProduction() const;
};

class MilitaryEscortShip: public Ship{
private:
    int numOfFighters = 0;
public:
    MilitaryEscortShip(string shipType);
    ~MilitaryEscortShip();

    int getNrProtected() const;
    void setFighters(int);
    int getNrFighters() const;
};

class Fleet{
private:
    string corporationName;
    int costOfFleet;
    int fleetWeight;
    int total_energyConsumption;
    bool medicStatus = false;
    long long distance_To_Gaia;
    int years_to_Gaia = 0;
    vector<Ship* > list_Ship;
    vector<ColonyShip* > list_ColonyShip;
    vector<MilitaryEscortShip* > list_MilitaryShip;
    vector<SolarSailShip* > list_SolarShip;

public:
    //constructor & destructor
    Fleet(string corporationName);
    ~Fleet();

    void setWeight(int);
    int getWeight() const;
    void setEnergyConsumption(int);
    int getEnergyConsumption() const;
    void setColonistCount(int);
    int getColonistCount() const;
    void setCost(int);
    int getCost() const;
    int EnergyProduction() const;
    int countProtectedShips() const;
    bool hasMedic() const;
    void setMedicStatus(bool);
    string getCorporationName() const;
    long long getVelocity() const;

    void addShip(Ship* );
    void add_ColonyShip(Ship* );
    void add_MilitaryShip(MilitaryEscortShip* );
    void add_SolarShip(SolarSailShip* );
    void shipDisplay();
    void displayProtectShip();
    void display_unProtectShip();
    void spaceAttack();
    void diseaseOutBreak();


    vector<Ship*> protectedShips() const;
    vector<Ship*> unprotectedShips() const;
    vector<Ship*> colonyShips() const;
    vector<Ship*> shipList() const;
    void destroyShip(Ship* i);

    void setDistanceToGaia();
    long long getDistanceToGaia();
    int getYearsToGaia();
};

class Gaia{
private:
    int population = 0;
    string colonyName;
public:
    Gaia();
    ~Gaia();

    void setPopulation(int);
    int getPopulation() const;
    void increasePopulation();
    void setColonyName(string);
    string getColonyName() const;
};

