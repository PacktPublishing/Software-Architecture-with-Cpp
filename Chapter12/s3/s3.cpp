#include <aws/core/Aws.h>
#include <aws/core/utils/UUID.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <string>

#include <spdlog/spdlog.h>

const Aws::S3::Model::BucketLocationConstraint region =
    Aws::S3::Model::BucketLocationConstraint::eu_central_1;

bool create_user_bucket(const std::string &username) {
  Aws::S3::Model::CreateBucketRequest request;

  Aws::String unique_prefix = Aws::Utils::UUID::RandomUUID();

  Aws::String bucket_name(("userbucket-" + username).c_str());
  Aws::String full_name = unique_prefix + bucket_name;
  request.SetBucket(Aws::Utils::StringUtils::ToLower(full_name.c_str()));

  Aws::S3::Model::CreateBucketConfiguration bucket_config;
  bucket_config.SetLocationConstraint(region);
  request.SetCreateBucketConfiguration(bucket_config);

  Aws::S3::S3Client s3_client;
  auto outcome = s3_client.CreateBucket(request);

  if (!outcome.IsSuccess()) {
    auto err = outcome.GetError();
    spdlog::error("ERROR:  CreateBucket:  {}:  {}", err.GetExceptionName(),
                  err.GetMessage());
    return false;
  }

  return true;
}

int main() {
  std::string username = "random-42";

  Aws::SDKOptions options;
  Aws::InitAPI(options);

  auto success = create_user_bucket(username);

  if (success) {
    std::cout << "The bucket for " << username << " is ready" << std::endl;
  }

  ShutdownAPI(options);
}
