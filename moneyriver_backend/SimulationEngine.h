#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

#include "DataStructures.h"
#include "TaxEngine.h"
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>

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
                activeSalary *= 1.07; // 7% annual step-up increment
            }

            double taxDeduction = TaxEngine::calculateMonthlyTax(activeSalary * 12.0, month, inflationRate);
            double netMonthlySalary = activeSalary - taxDeduction;
            double fdInterest = fdBalance * (0.071 / 12.0);
            
            bool isMarketShockActive = false;

            // Process Chronological Choice Injections & Shocks
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

            // Investment Deadlock Rule: Freeze investments if cashflow drops below zero (layoffs)
            double currentPrepay = isCashDry ? 0.0 : extraDebtPrepay;
            double currentStockSip = isCashDry ? 0.0 : stockSip;
            double currentMfSip = isCashDry ? 0.0 : mfSip;
            double currentFdAlloc = isCashDry ? 0.0 : fdAlloc;
            double currentGoldAlloc = isCashDry ? 0.0 : goldAlloc;
            double currentCryptoAlloc = isCashDry ? 0.0 : cryptoAlloc;

            // Amortization Waterfall Core (High-to-Low Interest Avalanche)
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

            // Stochastic Geometric Brownian Motion Asset Walks
            double stockVolatility = isMarketShockActive ? 0.42 : 0.15;
            double cryptoVolatility = isMarketShockActive ? 0.90 : 0.55;
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

            // Layer 5 Behavioral Sub-Metrics Calculations
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
#endif