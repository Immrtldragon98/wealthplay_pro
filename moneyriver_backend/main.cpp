#define _USE_MATH_DEFINES  // 💡 Add this first to unlock math constants on Windows
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cmath>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

namespace py = pybind11;

// ... rest of your uncompromised code continues exactly the same ...

namespace py = pybind11;

enum class LoanType { CAR, BIKE, STUDENT, GOLD, HOUSE, CREDIT_CARD };
enum class EventType { SUDDEN_LAYOFF, MEDICAL_EMERGENCY, STOCK_MARKET_CRASH, CLEAR_DEBT_FROM_EQUITY, START_NEW_FD };

struct CashFlows {
    double salary = 0.0;
    double rentalIncome = 0.0;
    double secondaryIncome = 0.0;
    double rentOut = 0.0;
    double ccBillLifestyle = 0.0;
    double travelOut = 0.0;
    double savingsTarget = 0.0;
    double fuelOut = 0.0;
    double subsOut = 0.0;
    double miscOut = 0.0;
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

class TaxEngine {
public:
    static double calculateMonthlyTax(double annualSalary, int monthIndex, double inflationRate) {
        double standardDeduction = 75000.0;
        double taxableIncome = std::max(0.0, annualSalary - standardDeduction);
        double inflationFactor = std::pow(1.0 + (inflationRate / 100.0), std::floor((monthIndex - 1) / 12.0));
        
        double cumulativeTax = 0.0;
        if (taxableIncome > 400000 * inflationFactor)  cumulativeTax += (std::min(taxableIncome, 800000 * inflationFactor) - 400000 * inflationFactor) * 0.05;
        if (taxableIncome > 800000 * inflationFactor)  cumulativeTax += (std::min(taxableIncome, 1200000 * inflationFactor) - 800000 * inflationFactor) * 0.10;
        if (taxableIncome > 1200000 * inflationFactor) cumulativeTax += (std::min(taxableIncome, 1600000 * inflationFactor) - 1200000 * inflationFactor) * 0.15;
        if (taxableIncome > 1600000 * inflationFactor) cumulativeTax += (std::min(taxableIncome, 2000000 * inflationFactor) - 1600000 * inflationFactor) * 0.20;
        if (taxableIncome > 2000000 * inflationFactor) cumulativeTax += (taxableIncome - 2000000 * inflationFactor) * 0.30;
        
        return cumulativeTax / 12.0;
    }
};

class SimulationEngine {
private:
    static double getGaussianRandom() {
        static std::mt19937 generator(42); 
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double u = 0.0, v = 0.0;
        while (u == 0.0) u = distribution(generator);
        while (v == 0.0) v = distribution(generator);
        return std::sqrt(-2.0 * std::log(u)) * std::cos(2.0 * M_PI * v);
    }

public:
    static std::vector<MonthSnapshot> runSimulation(
        int horizonYears, double inflationRate, CashFlows cf,
        std::vector<Loan> userLoans, std::vector<FutureEvent> events,
        double extraDebtPrepay, double stockSip, double mfSip, 
        double fdAlloc, double goldAlloc, double cryptoAlloc, double luxuryWishlistCost
    ) {
        int totalMonths = horizonYears * 12;
        std::vector<MonthSnapshot> records;
        
        double monthlyInflationFactor = (inflationRate / 100.0) / 12.0;
        const int totalTrials = 100;
        
        std::vector<double> trialMatrixEquity(totalTrials, 0.0);
        std::vector<double> trialMatrixCrypto(totalTrials, 0.0);
        double fdBalance = 0.0;
        double goldBalance = 0.0;
        double activeSalary = cf.salary;

        for (int month = 1; month <= totalMonths; ++month) {
            if (month > 1 && (month - 1) % 12 == 0) {
                activeSalary *= 1.07; 
            }

            double taxDeduction = TaxEngine::calculateMonthlyTax(activeSalary * 12.0, month, inflationRate);
            double netMonthlySalary = activeSalary - taxDeduction;
            double fdInterest = fdBalance * (0.071 / 12.0);
            
            bool isMarketShockActive = false;

            for (const auto& evt : events) {
                if (evt.targetMonth == month) {
                    if (evt.type == EventType::SUDDEN_LAYOFF) netMonthlySalary = 0.0;
                    else if (evt.type == EventType::STOCK_MARKET_CRASH) {
                        isMarketShockActive = true;
                        for (int t = 0; t < totalTrials; ++t) trialMatrixEquity[t] *= 0.70;
                    } else if (evt.type == EventType::START_NEW_FD) {
                        fdBalance += evt.value;
                    } else if (evt.type == EventType::CLEAR_DEBT_FROM_EQUITY) {
                        std::sort(userLoans.begin(), userLoans.end(), [](const Loan& a, const Loan& b) { return a.rate > b.rate; });
                        for (int t = 0; t < totalTrials; ++t) {
                            double liquidationPool = trialMatrixEquity[t] * evt.liquidationRatio;
                            trialMatrixEquity[t] *= (1.0 - evt.liquidationRatio);
                            for (auto& loan : userLoans) {
                                if (loan.principal > 0 && liquidationPool > 0) {
                                    if (liquidationPool >= loan.principal) {
                                        liquidationPool -= loan.principal;
                                        loan.principal = 0;
                                    } else {
                                        loan.principal -= liquidationPool;
                                        liquidationPool = 0;
                                    }
                                }
                            }
                            trialMatrixEquity[t] += liquidationPool;
                        }
                    }
                }
            }

            double totalExpenses = cf.rentOut + cf.ccBillLifestyle + cf.travelOut + 
                                  cf.savingsTarget + cf.fuelOut + cf.subsOut + cf.miscOut;
            
            double emiOverhead = 0.0;
            for (const auto& loan : userLoans) {
                if (loan.principal > 0) emiOverhead += loan.emi;
            }

            double surplus = (netMonthlySalary + cf.rentalIncome + cf.secondaryIncome + fdInterest) - totalExpenses - emiOverhead;
            bool isCashDry = (surplus <= 0);

            double currentPrepay = isCashDry ? 0.0 : extraDebtPrepay;
            double currentStockSip = isCashDry ? 0.0 : stockSip;
            double currentMfSip = isCashDry ? 0.0 : mfSip;
            double currentFdAlloc = isCashDry ? 0.0 : fdAlloc;
            double currentGoldAlloc = isCashDry ? 0.0 : goldAlloc;
            double currentCryptoAlloc = isCashDry ? 0.0 : cryptoAlloc;

            double extraCashPool = currentPrepay;
            std::sort(userLoans.begin(), userLoans.end(), [](const Loan& a, const Loan& b) { return a.rate > b.rate; });
            
            double totalRemainingDebt = 0.0;
            for (auto& loan : userLoans) {
                if (loan.principal > 0) {
                    double interestCost = (loan.principal * (loan.rate / 100.0)) / 12.0;
                    double principalReduction = loan.emi - interestCost;
                    
                    if (extraCashPool > 0) {
                        principalReduction += extraCashPool;
                        if (loan.principal - principalReduction <= 0) {
                            extraCashPool = std::abs(loan.principal - principalReduction);
                            loan.principal = 0;
                        } else {
                            loan.principal -= principalReduction;
                            extraCashPool = 0;
                        }
                    } else {
                        loan.principal = std::max(0.0, loan.principal - principalReduction);
                    }
                }
                totalRemainingDebt += loan.principal;
            }

            // Vectorized Variances across Asset Categories
            double stockVolatility = isMarketShockActive ? 0.42 : 0.16;
            double cryptoVolatility = isMarketShockActive ? 0.90 : 0.60;
            double stockInput = currentStockSip + currentMfSip;
            if (totalRemainingDebt == 0) stockInput += (emiOverhead + currentPrepay);

            for (int t = 0; t < totalTrials; ++t) {
                double stockNoise = getGaussianRandom();
                double cryptoNoise = getGaussianRandom();
                trialMatrixEquity[t] = (trialMatrixEquity[t] + stockInput) * std::exp((0.12 - (stockVolatility * stockVolatility) / 2.0) * (1.0/12.0) + stockVolatility * std::sqrt(1.0/12.0) * stockNoise);
                trialMatrixCrypto[t] = (trialMatrixCrypto[t] + currentCryptoAlloc) * std::exp((0.25 - (cryptoVolatility * cryptoVolatility) / 2.0) * (1.0/12.0) + cryptoVolatility * std::sqrt(1.0/12.0) * cryptoNoise);
            }

            fdBalance = (fdBalance + currentFdAlloc) * (1.0 + (0.071 / 12.0));
            goldBalance = (goldBalance + currentGoldAlloc) * (1.0 + (0.085 / 12.0));

            std::sort(trialMatrixEquity.begin(), trialMatrixEquity.end());
            std::sort(trialMatrixCrypto.begin(), trialMatrixCrypto.end());

            double medianWealth = trialMatrixEquity[totalTrials / 2] + trialMatrixCrypto[totalTrials / 2] + fdBalance + goldBalance;
            double p10Wealth = trialMatrixEquity[totalTrials * 10 / 100] + trialMatrixCrypto[totalTrials * 10 / 100] + fdBalance + goldBalance;
            double p90Wealth = trialMatrixEquity[totalTrials * 90 / 100] + trialMatrixCrypto[totalTrials * 90 / 100] + fdBalance + goldBalance;
            double realWealth = (medianWealth - totalRemainingDebt) / std::pow(1.0 + monthlyInflationFactor, month);

            int stress = std::min(100, int((totalRemainingDebt / 40000.0) + (surplus < 15000 ? 40 : 0)));
            int pressure = cf.travelOut == 0 ? 60 : std::min(100, int((cf.savingsTarget / (cf.salary + 1)) * 120));
            int regret = (extraDebtPrepay > (cf.salary * 0.25) && cf.travelOut < 3000) ? 85 : 20;

            records.push_back({
                month, totalRemainingDebt, medianWealth, realWealth, p10Wealth, p90Wealth, surplus + (fdBalance * 0.5), luxuryWishlistCost, stress, pressure, regret
            });
        }
        return records;
    }
};

PYBIND11_MODULE(moneyriver_core, m) {
    m.doc() = "Uncompromised High-Performance C++ Computational Core";

    py::enum_<LoanType>(m, "LoanType")
        .value("CAR", LoanType::CAR).value("BIKE", LoanType::BIKE).value("STUDENT", LoanType::STUDENT)
        .value("GOLD", LoanType::GOLD).value("HOUSE", LoanType::HOUSE).value("CREDIT_CARD", LoanType::CREDIT_CARD);

    py::enum_<EventType>(m, "EventType")
        .value("SUDDEN_LAYOFF", EventType::SUDDEN_LAYOFF).value("STOCK_MARKET_CRASH", EventType::STOCK_MARKET_CRASH)
        .value("MEDICAL_EMERGENCY", EventType::MEDICAL_EMERGENCY).value("CLEAR_DEBT_FROM_EQUITY", EventType::CLEAR_DEBT_FROM_EQUITY)
        .value("START_NEW_FD", EventType::START_NEW_FD);

    py::class_<CashFlows>(m, "CashFlows")
        .def(py::init<>())
        .def_readwrite("salary", &CashFlows::salary).def_readwrite("rentalIncome", &CashFlows::rentalIncome).def_readwrite("secondaryIncome", &CashFlows::secondaryIncome)
        .def_readwrite("rentOut", &CashFlows::rentOut).def_readwrite("ccBillLifestyle", &CashFlows::ccBillLifestyle).def_readwrite("travelOut", &CashFlows::travelOut)
        .def_readwrite("savingsTarget", &CashFlows::savingsTarget).def_readwrite("fuelOut", &CashFlows::fuelOut).def_readwrite("subsOut", &CashFlows::subsOut)
        .def_readwrite("miscOut", &CashFlows::miscOut);

    py::class_<Loan>(m, "Loan")
        .def(py::init<int, std::string, LoanType, double, double, double>())
        .def_readwrite("id", &Loan::id).def_readwrite("name", &Loan::name).def_readwrite("type", &Loan::type)
        .def_readwrite("principal", &Loan::principal).def_readwrite("emi", &Loan::emi).def_readwrite("rate", &Loan::rate);

    py::class_<FutureEvent>(m, "FutureEvent")
        .def(py::init<int, int, EventType, double, double, double, std::string>())
        .def_readwrite("id", &FutureEvent::id).def_readwrite("targetMonth", &FutureEvent::targetMonth).def_readwrite("type", &FutureEvent::type)
        .def_readwrite("value", &FutureEvent::value).def_readwrite("emiValue", &FutureEvent::emiValue)
        .def_readwrite("liquidationRatio", &FutureEvent::liquidationRatio).def_readwrite("description", &FutureEvent::description);

    py::class_<MonthSnapshot>(m, "MonthSnapshot")
        .def_readwrite("month", &MonthSnapshot::month).def_readwrite("outstandingDebt", &MonthSnapshot::outstandingDebt)
        .def_readwrite("nominalWealth", &MonthSnapshot::nominalWealth).def_readwrite("realWealth", &MonthSnapshot::realWealth)
        .def_readwrite("p10Wealth", &MonthSnapshot::p10Wealth).def_readwrite("p90Wealth", &MonthSnapshot::p90Wealth)
        .def_readwrite("survivalCapacity", &MonthSnapshot::survivalCapacity).def_readwrite("luxuryTargetBoundary", &MonthSnapshot::luxuryTargetBoundary)
        .def_readwrite("stressIndex", &MonthSnapshot::stressIndex).def_readwrite("pressureIndex", &MonthSnapshot::pressureIndex).def_readwrite("regretIndex", &MonthSnapshot::regretIndex);

    m.def("runSimulation", &SimulationEngine::runSimulation, "Execute stochastic simulation path matrices");
}