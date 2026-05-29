#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>

enum class LoanType { CAR, BIKE, STUDENT, GOLD, HOUSE, CREDIT_CARD };
enum class EventType { SUDDEN_LAYOFF, MEDICAL_EMERGENCY, STOCK_MARKET_CRASH, CLEAR_DEBT_FROM_EQUITY, START_NEW_FD };

struct CashFlows {
    double salary;
    double rentalIncome;
    double secondaryIncome;
    double rentOut;
    double ccBillLifestyle;
    double travelOut;
    double savingsTarget;
    double fuelOut;
    double subsOut;
    double miscOut;
};

struct Loan {
    int id;
    std::string name;
    LoanType type;
    double principal;
    double emi;
    double rate;
};

struct FutureEvent {
    int id;
    int targetMonth;
    EventType type;
    double value;
    double emiValue;
    double liquidationRatio;
    std::string description;
};

struct MonthSnapshot {
    int month;
    double outstandingDebt;
    double nominalWealth;
    double realWealth;
    double p10Wealth;
    double p90Wealth;
    double survivalCapacity;
    double luxuryTargetBoundary;
    int stressIndex;
    int pressureIndex;
    int regretIndex;
};

#endif