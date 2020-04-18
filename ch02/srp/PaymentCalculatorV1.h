#pragma once

class PaymentCalculator;
{
 public:
  double calculate() const;

  void setHours(double hours);

  void setHourlyRate(double rate);

  void setTaxPercentage(double tax);

 private:
  double hours_;
  double netHourlyRate_;
  double taxPercentage_;
};
