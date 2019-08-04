#pragma once

#include <gsl/pointers> // contains gsl::not_null

class Consultant;

class PaymentCalculator {
public:
    double calculate() const;

    void setConsultant(const Consultant &c);

    void setTaxPercentage(double tax);

private:
    gsl::not_null<const Consultant *> consultant_;
    double taxPercentage_;
};
