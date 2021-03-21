#pragma once

#include <gsl/pointers>  // contains gsl::not_null

class Consultant;

class PaymentCalculator {
 public:
  double calculate() const { return {}; }

  void setConsultant(const Consultant &c) { consultant_ = &consultant_; }

  void setTaxPercentage(double tax) { taxPercentage_ = tax; }

 private:
  gsl::not_null<const Consultant *> consultant_;
  double taxPercentage_;
};
