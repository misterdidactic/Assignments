#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void write_to_file(
    string fname,
    string lname,
    double weekly_wage,
    double adj_weekly_wage,
    double tax_payable,
    double usc_payable,
    double prsi_payable,
    double pension_payable,
    double net_income
)
{
    string filename;
    // generate filename from employee's name
    filename += "payroll/" + lname + "_" + fname + ".txt";
    // create file and specify to write
    ofstream payrollfile(filename, ios::out);
    // if the file can be created & opened
    if (payrollfile.is_open()) 
    { 
        // ok, proceed with output
        payrollfile << "-----------------------------------------------------------------" << endl;
        payrollfile << lname << ", " << fname << endl;
        payrollfile << "Gross Income: " << weekly_wage << endl;
        payrollfile << "Adj. Income: " << adj_weekly_wage << endl;
        payrollfile << "Tax payable: " << tax_payable << endl;
        payrollfile << "USC: " << usc_payable << endl;
        payrollfile << "PRSI: " << prsi_payable << endl;
        payrollfile << "Pension payment: " << pension_payable << endl;
        payrollfile << "Net Income (after tax, USC, PRSI and pension): " << net_income << endl;
        payrollfile << "-----------------------------------------------------------------" << endl;
        cout << "Finished " << lname << ", " << fname << endl;
    }
    // if the file cannot be created/opened
    else
    {
        cerr << "Failed to open file. Exiting out..." << endl;
        exit(0); //failed to open file
    }
    payrollfile.close();
}

void calculate_taxes(
    string fname,
    string lname,
    string marital_status,
    int age,
    double weekly_wage,
    double pension,
    double tax_credit,
    double tax_allowance,
    char full_medical_card
)
{
    // Define variables
    double tax_payable;
    double usc_payable = 0;
    double prsi_payable = 0;
    double pension_payable;
    double net_income;
    double adj_weekly_wage = weekly_wage - tax_allowance;
    double lower_band;

    // calculation is changed based on marital status
    // s: single
    // m1: married w/ one earner
    // m2: married w/ two earners
    // o: one parent family
    if (marital_status == "s")
    {
        lower_band = 650;
    }
    else if (marital_status == "m1")
    {
        lower_band = 823.08;
    }
    else if (marital_status == "m2")
    {
        lower_band = 823.08;
    }
    else if (marital_status == "o")
    {
        lower_band = 726.92;
    }


    if (adj_weekly_wage >= lower_band)
    {
        // get remainder for after first band
        double rmndr = adj_weekly_wage - lower_band;
        // calculate total tax for both bands minus tax credit
        tax_payable = lower_band*0.2 + rmndr*0.4 - tax_credit;
    }
    else if (adj_weekly_wage < lower_band)
    {
        // calculate tax for lower bound only
        tax_payable = lower_band*0.2 - tax_credit;
    }
    if (weekly_wage > 231)
    {
        // calculate initial usc
        usc_payable += 231*0.005;
    }
    if (weekly_wage > 141.54) 
    {
        // having a full medical card means not having to pay USC
        // beyond a certain point 
        usc_payable += 141.54*0.005;
        if (full_medical_card == 'n')
        {
            usc_payable += (weekly_wage-141.54)*0.475;
        }
    }
    prsi_payable = weekly_wage*0.04;
    net_income = weekly_wage - usc_payable - prsi_payable - tax_payable;
    pension_payable = net_income*0.01*pension;
    net_income -= pension_payable;
    
    write_to_file(
        fname,
        lname,
        weekly_wage,
        adj_weekly_wage,
        tax_payable,
        usc_payable,
        prsi_payable,
        pension_payable,
        net_income
    );
}

void read_csv()
{
    string filename;
    cout << "Name of csv file (NOTE the csv must be delimited using spaces, not commas): ";
    cin >> filename;
    ifstream infile(filename);
    if (infile.is_open()) {
        string fname;
        string lname;
        string marital_status;
        int age;
        double weekly_wage;
        double pension;
        double tax_credit;
        double tax_allowance;
        char full_medical_card;
        char sep = ' ';

        string line;
        while (!infile.eof())
        {
            getline(infile, line);
            istringstream iss(line);
            if (!(
                iss >> 
                fname >>
                lname >>
                marital_status >>
                age >>
                weekly_wage >>
                pension >>
                tax_credit >>
                tax_allowance >>
                full_medical_card
            )) 
            { break; } // error

            calculate_taxes( 
                fname,
                lname,
                marital_status,
                age,
                weekly_wage,
                pension,
                tax_credit,
                tax_allowance,
                full_medical_card
            );
        }
        infile.close();
    }
    else
    {
        cout << "Error with that file. Try again!" << endl;
    }
}

void manual_input()
{
    string fname;
    string lname;
    string marital_status;
    int age;
    double weekly_wage;
    double pension;
    double tax_credit;
    double tax_allowance;
    char full_medical_card;
    cout << "First Name: ";
    cin >> fname;
    cout << "Last Name: ";
    cin >> lname;

    cout << "marital status (s: single, m1: married with one income, m2: married with two incomes, o: one parent family): ";
    cin >> marital_status;
    cout << "age: ";
    cin >> age;
    cout << "weekly wage: ";
    cin >> weekly_wage;
    cout << "pension: ";
    cin >> pension;
    cout << "tax credit: ";
    cin >> tax_credit;
    cout << "tax allowance: ";
    cin >> tax_allowance;
    cout << "full medical card (y/n): ";
    cin >> full_medical_card;
    calculate_taxes( 
        fname,
        lname,
        marital_status,
        age,
        weekly_wage,
        pension,
        tax_credit,
        tax_allowance,
        full_medical_card
    );
}

int main()
{
    bool keepgoing = true;
    char manual_or_csv;
    while (keepgoing)
    {
        cout << "Manual input (m), csv file (f) or exit (e) : ";
        cin >> manual_or_csv;
        if (manual_or_csv == 'f')
        {
            read_csv();
        }
        else if (manual_or_csv == 'm')
        {
            manual_input();
        }
        else if (manual_or_csv == 'e')
        {
            keepgoing = false;
        }
    }
    cout << "Press any key to exit..." << endl;
    cin.ignore();
    cin.get();
    return 0;
}