#pragma once

class PaymentCalculator;
{
 public:
  double calculate() const { return {/* here be accounting wizardry */}; }

  void setHours(double hours) { hours_ = hours; }

  void setHourlyRate(double rate) { netHourlyRate_ = rate; }

  void setTaxPercentage(double tax) { taxPercentage_ = tax; }

 private:
  double hours_;
  double netHourlyRate_;
  double taxPercentage_;
};
