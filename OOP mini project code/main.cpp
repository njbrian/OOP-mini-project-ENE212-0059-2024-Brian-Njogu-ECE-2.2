#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <map>

using namespace std;

// THEE PRODUCT CLASS
class Product {
    string name;
    double price;
    int stock;
public:
    Product(string n, double p, int s) {
        name = n;
        price = p;
        stock = s;
    }

    string getName() const {
        return name;
    }
    double getPrice() const {
        return price;
    }
    int getStock() const {
        return stock;
    }

    void reduceStock (int qty) {
        if (stock >= qty) {
            stock = stock - qty;
        }
    }
    void restock(int qty) {
        stock = stock + qty;
    }
};

// THEE TRANSACTION CLASS
class Transaction {
    string productName;
    double amount;
    int quantity;
public:
    Transaction(string p, double a, int q) {
        productName = p;
        amount = a;
        quantity = q;
    }

    void printDetails() const {
        cout << "Purchased: " << quantity << " x " << productName << " for Ksh: " << amount << endl;
    }
    double getAmount() const {
        return amount;
    }
    string getProductName() const {
        return productName;
    }
    int getQuantity() const {
        return quantity;
    }
};

// THEE PAYMENT CLASS (ABSTRACT BASE CLASS)

class Payment {
public:
    virtual bool processPayment(double amount) = 0;
    virtual ~Payment() {}
};

// THE CHILD CLASS CASH PAYMENT (INHERITS FROM BASE CLASS PAYMENT)

class CashPayment : public Payment  {
    double cashInserted;
public:
    CashPayment(double c) {
        cashInserted = c;
    }

    bool processPayment(double amount) override {
        if (cashInserted >= amount) {
            cout << "Payment successful. Change, Ksh: " << cashInserted - amount << endl;
            return true;
        }
        cout << "Insufficient funds." << endl;
        return false;
    }
};

// THE CHILD CLASS CARD PAYMENT --INHERITS FROM PAYMENT PARENT CLASS

class CardPayment : public Payment {
public:
    bool processPayment(double amount) override {
        cout << "Card charged, Ksh: " << amount << endl;
        return true;
    }
};

// THE CLASS MPESA PAYMENT INHERITS FROM PAYMENT PARENT CLASS

class MobileMoneyPayment : public Payment {
    string phoneNumber;
public:
    MobileMoneyPayment(string phone) {
        phoneNumber = phone;
    }
    bool processPayment(double amount) override {
        cout << "M-Pesa payment request sent to " << phoneNumber << endl;
        cout << "Payment of Ksh: " << amount << " confirmed via M-Pesa." << endl;
        return true;
    }
};

// THEE VENDING MACHINE CLASS

class VendingMachine {
    vector<Product> products;
    vector<Transaction> history;
public:
    void addProduct(const Product& p) {
        products.push_back(p);
    }
    void showProducts() const {
        cout << "\nAvailable Products:\n";
        for(size_t i=0; i<products.size(); i++) {
            cout << i+1 << ". " << products[i].getName()
                 << " - " << products[i].getPrice()
                 << " (" << products[i].getStock() << " left)\n";
        }
    }
    void purchaseProduct(int index, int qty, Payment& payment) {
        if(index < 1 || index > (int)products.size()) {
            cout << "Invalid selection.\n"; return;
        }
        Product& p = products[index-1];
        if(p.getStock() < qty) {
            cout << "Not enough stock.\n";
            return;
        }
        double totalPrice = p.getPrice() * qty;
        if(payment.processPayment(totalPrice)) {
            p.reduceStock(qty);
            Transaction t(p.getName(), totalPrice, qty);
            history.push_back(t);
            saveSalesToFile(t);
        }
    }
    void generateReport() const {
        cout << "\nSales Report:\n";
        double total = 0;
        map<string,int> productCount;
        for(auto &t : history) {
            t.printDetails();
            total += t.getAmount();
            productCount[t.getProductName()] += t.getQuantity();
        }
        cout << "\nPer-Product Sales Summary:\n";
        for(auto &entry : productCount) {
            cout << entry.first << " sold: " << entry.second << " units\n";
        }

        cout << "\nTotal Revenue, Ksh: " << total << endl;
    }
    void restockProduct(int index, int qty) {
        if(index < 1 || index > (int)products.size()) {
            cout << "Invalid product.\n"; return;
        }
        products[index-1].restock(qty);
        cout << "Restocked " << products[index-1].getName() << " by " << qty << endl;
    }
    void saveSalesToFile(const Transaction& t) const {
        ofstream file("sales.txt", ios::app);
        file << t.getProductName() << "," << t.getAmount() << "," << t.getQuantity() << endl;
        file.close();
    }
    void loadSalesFromFile() {
        ifstream file("sales.txt");
        string line;
        while(getline(file, line)) {
            size_t pos1 = line.find(",");
            size_t pos2 = line.find(",", pos1+1);
            if(pos1 != string::npos && pos2 != string::npos) {
                string name = line.substr(0, pos1);
                double amount = stod(line.substr(pos1+1, pos2-pos1-1));
                int qty = stoi(line.substr(pos2+1));
                history.emplace_back(name, amount, qty);
            }
        }
        file.close();
    }
};

// THEE MAIN CLASS

int main() {
    VendingMachine vm;
    vm.addProduct(Product("Soda", 50.0, 5));
    vm.addProduct(Product("Chips", 30.0, 3));
    vm.addProduct(Product("Chocolate", 40.0, 2));
    vm.addProduct(Product("Water", 20.0, 10));
    vm.addProduct(Product("Juice", 60.0, 4));
    vm.addProduct(Product("Biscuits", 25.0, 6));
    vm.addProduct(Product("Sandwich", 80.0, 3));

    vm.loadSalesFromFile();

    int mode;
    do {
        cout << "\nMain Menu\n1. Buyer Mode\n2. Admin Mode\n3. Exit\nChoice: ";
        cin >> mode;

        if(mode == 1) {
            int choice;
            do {
                cout << "\nBuyer Menu\n1. Show Products\n2. Buy Product\n3. Back to Main Menu\nChoice: ";
                cin >> choice;
                if(choice == 1) vm.showProducts();
                else if(choice == 2) {
                    int index, qty;
                    vm.showProducts();
                    cout << "Select product number: "; cin >> index;
                    cout << "Enter quantity: "; cin >> qty;

                    int payChoice;
                    cout << "Select Payment Method:\n1. Cash\n2. Card\n3. Mobile Money (M-Pesa)\nChoice: ";
                    cin >> payChoice;

                    if(payChoice == 1) {
                        double cash;
                        cout << "Insert cash: "; cin >> cash;
                        CashPayment cp(cash);
                        vm.purchaseProduct(index, qty, cp);
                    } else if(payChoice == 2) {
                        CardPayment cp;
                        vm.purchaseProduct(index, qty, cp);
                    } else if(payChoice == 3) {
                        string phone;
                        cout << "Enter M-Pesa phone number: "; cin >> phone;
                        MobileMoneyPayment mpesa(phone);
                        vm.purchaseProduct(index, qty, mpesa);
                    } else {
                        cout << "Invalid payment method.\n";
                    }
                }
            } while(choice != 3);
        }
        else if(mode == 2) {
            int choice;
            do {
                cout << "\nAdmin Menu\n1. View Sales Report\n2. Restock Product\n3. Back to Main Menu\nChoice: ";
                cin >> choice;
                if(choice == 1) vm.generateReport();
                else if(choice == 2) {
                    int index, qty;
                    vm.showProducts();
                    cout << "Select product number: "; cin >> index;
                    cout << "Enter restock quantity: "; cin >> qty;
                    vm.restockProduct(index, qty);
                }
            } while(choice != 3);
        }
    } while(mode != 3);

    cout << "\nExiting program...\n";
    return 0;
};
