#ifndef TAX_ENGINE_H
#define TAX_ENGINE_H
#include <algorithm>
#include <cmath>

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
#endif