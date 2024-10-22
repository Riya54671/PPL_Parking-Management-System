#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <queue>
using namespace std;

// Base class for vehicles
class Vehicle {
protected:
    string vehicleNumber;
    time_t entryTime;
    time_t exitTime;

public:
    Vehicle(string vehicleNo) : vehicleNumber(vehicleNo) {
        time(&entryTime);  // Set entry time when the vehicle is created
    }

    virtual ~Vehicle() {}

    virtual string getType() const = 0; // Pure virtual function for polymorphism
    string getVehicleNumber() const {
        return vehicleNumber;
    }

    void setExitTime() {
        time(&exitTime); // Set exit time when the vehicle leaves
    }

    double getParkingDuration() const {
        return difftime(exitTime, entryTime) / 3600.0; // Calculate hours parked
    }

    virtual double calculateParkingFee() const = 0; // Abstract method for fee calculation
};

// Derived class for cars
class Car : public Vehicle {
public:
    Car(string vehicleNo) : Vehicle(vehicleNo) {}

    string getType() const override {
        return "Car";
    }

    double calculateParkingFee() const override {
        double parkingRate = 10.0; // 10 units per hour for cars
        return getParkingDuration() * parkingRate;
    }
};

// Derived class for bikes
class Bike : public Vehicle {
public:
    Bike(string vehicleNo) : Vehicle(vehicleNo) {}

    string getType() const override {
        return "Bike";
    }

    double calculateParkingFee() const override {
        double parkingRate = 5.0; // 5 units per hour for bikes
        return getParkingDuration() * parkingRate;
    }
};

// Base class for parking slots
class ParkingSlot {
protected:
    int slotNumber;
    bool isOccupied;
    Vehicle* vehicle;

public:
    ParkingSlot(int slotNo) : slotNumber(slotNo), isOccupied(false), vehicle(nullptr) {}

    virtual ~ParkingSlot() {
        delete vehicle;
    }

    virtual string getSlotType() const = 0; // Abstract method to get slot type

    bool isSlotOccupied() const {
        return isOccupied;
    }

    int getSlotNumber() const {
        return slotNumber;
    }

    void parkVehicle(Vehicle* v) {
        isOccupied = true;
        vehicle = v;
        cout << v->getType() << " " << v->getVehicleNumber() << " parked in slot " << slotNumber << endl;
    }

    void removeVehicle() {
        if (isOccupied && vehicle) {
            vehicle->setExitTime();
            cout << vehicle->getType() << " " << vehicle->getVehicleNumber() 
                 << " left slot " << slotNumber << endl;
            cout << "Total parking fee: " << fixed << setprecision(2) 
                 << vehicle->calculateParkingFee() << " units." << endl;
            isOccupied = false;
            delete vehicle;
            vehicle = nullptr;
        } else {
            cout << "No vehicle in slot " << slotNumber << endl;
        }
    }

    void displaySlotInfo() const {
        if (isOccupied && vehicle) {
            cout << "Slot " << slotNumber << " is occupied by " << vehicle->getType()
                 << " " << vehicle->getVehicleNumber() << endl;
        } else {
            cout << "Slot " << slotNumber << " is available." << endl;
        }
    }
};

// Derived class for car slots
class CarSlot : public ParkingSlot {
public:
    CarSlot(int slotNo) : ParkingSlot(slotNo) {}

    string getSlotType() const override {
        return "CarSlot";
    }
};

// Derived class for bike slots
class BikeSlot : public ParkingSlot {
public:
    BikeSlot(int slotNo) : ParkingSlot(slotNo) {}

    string getSlotType() const override {
        return "BikeSlot";
    }
};

// Parking lot class with inheritance and polymorphism
class ParkingLot {
private:
    ParkingSlot** slots;
    int totalSlots;
    int availableCarSlots;
    int availableBikeSlots;
    queue<Vehicle*> waitlist;

public:
    ParkingLot(int carSlots, int bikeSlots) {
        totalSlots = carSlots + bikeSlots;
        slots = new ParkingSlot*[totalSlots];
        availableCarSlots = carSlots;
        availableBikeSlots = bikeSlots;

        // Initialize car slots
        for (int i = 0; i < carSlots; i++) {
            slots[i] = new CarSlot(i + 1);
        }

        // Initialize bike slots
        for (int i = carSlots; i < totalSlots; i++) {
            slots[i] = new BikeSlot(i + 1);
        }
    }

    ~ParkingLot() {
        for (int i = 0; i < totalSlots; i++) {
            delete slots[i];
        }
        delete[] slots;
    }

    void parkVehicle(Vehicle* v) {
        if (v->getType() == "Car" && availableCarSlots > 0) {
            for (int i = 0; i < availableCarSlots; i++) {
                if (!slots[i]->isSlotOccupied()) {
                    slots[i]->parkVehicle(v);
                    return;
                }
            }
        } else if (v->getType() == "Bike" && availableBikeSlots > 0) {
            for (int i = availableCarSlots; i < totalSlots; i++) {
                if (!slots[i]->isSlotOccupied()) {
                    slots[i]->parkVehicle(v);
                    return;
                }
            }
        } else {
            cout << "No available slot for " << v->getType() << ". Adding to waitlist.\n";
            waitlist.push(v);
        }
    }

    void removeVehicle(int slotNo) {
        if (slotNo > 0 && slotNo <= totalSlots) {
            slots[slotNo - 1]->removeVehicle();
            handleWaitlist();
        } else {
            cout << "Invalid slot number." << endl;
        }
    }

    void handleWaitlist() {
        if (!waitlist.empty()) {
            Vehicle* nextVehicle = waitlist.front();
            waitlist.pop();
            parkVehicle(nextVehicle);
        }
    }

    void displayParkingLotInfo() {
        for (int i = 0; i < totalSlots; i++) {
            slots[i]->displaySlotInfo();
        }
    }
};

int main() {
    int carSlots, bikeSlots;
    cout << "Enter the number of car slots: ";
    cin >> carSlots;
    cout << "Enter the number of bike slots: ";
    cin >> bikeSlots;

    ParkingLot parkingLot(carSlots, bikeSlots);

    int choice;
    do {
        cout << "\n1. Park Vehicle\n2. Remove Vehicle\n3. Display Parking Lot Info\n4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string type, vehicleNumber;
            cout << "Enter vehicle type (Car/Bike): ";
            cin >> type;
            cout << "Enter vehicle number: ";
            cin >> vehicleNumber;

            if (type == "Car") {
                Vehicle* car = new Car(vehicleNumber);
                parkingLot.parkVehicle(car);
            } else if (type == "Bike") {
                Vehicle* bike = new Bike(vehicleNumber);
                parkingLot.parkVehicle(bike);
            } else {
                cout << "Invalid vehicle type!" << endl;
            }
        } else if (choice == 2) {
            int slotNumber;
            cout << "Enter slot number to remove vehicle: ";
            cin >> slotNumber;
            parkingLot.removeVehicle(slotNumber);
        } else if (choice == 3) {
            parkingLot.displayParkingLotInfo();
        }

    } while (choice != 4);

    return 0;
}
