/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <gtest/gtest.h>
#include "gflags/gflags.h"
#include "test_helper.h"

DEFINE_string(dirname, "", "Directory of the inference model.");

// Test for testing inference using "decode_main" in python
TEST(inference, machine_translation_decode) {
  if (FLAGS_dirname.empty()) {
    LOG(FATAL) << "Usage: ./example --dirname=path/to/your/model";
  }

  LOG(INFO) << "FLAGS_dirname: " << FLAGS_dirname << std::endl;
  std::string dirname = FLAGS_dirname;

  // 0. Call `paddle::framework::InitDevices()` initialize all the devices
  // In unittests, this is done in paddle/testing/paddle_gtest_main.cc

  // Setup init_ids and init_scores
  paddle::framework::LoDTensor init_ids, init_scores;
  paddle::framework::LoD init_lod{{0, 1}};
  init_lod.push_back({0, 1});

  std::vector<int64_t> init_ids_data = {1};
  SetupLoDTensor<int64_t>(init_ids, {1, 1}, init_lod, init_ids_data);

  std::vector<float> init_scores_data = {1};
  SetupLoDTensor<float>(init_scores, {1, 1}, init_lod, init_scores_data);

  // Setup input sequence of 5 words
  paddle::framework::LoDTensor input_sequence;
  std::vector<int64_t> inp_data = {100, 50, 8, 94, 122};
  SetupLoDTensor(input_sequence, {5, 1}, {{0, 5}}, inp_data);

  std::vector<paddle::framework::LoDTensor*> cpu_feeds;
  cpu_feeds.push_back(&input_sequence);
  cpu_feeds.push_back(&init_ids);
  cpu_feeds.push_back(&init_scores);

  paddle::framework::LoDTensor result_ids1, result_scores1;
  std::vector<paddle::framework::LoDTensor*> cpu_fetchs1;
  cpu_fetchs1.push_back(&result_ids1);
  cpu_fetchs1.push_back(&result_scores1);

  // Run inference on CPU
  TestInference<paddle::platform::CPUPlace>(dirname, cpu_feeds, cpu_fetchs1);
  LOG(INFO) << result_ids1.lod();
  LOG(INFO) << result_ids1.dims();
  LOG(INFO) << result_scores1.lod();
  LOG(INFO) << result_scores1.dims();

#ifdef PADDLE_WITH_CUDA
  LOG(INFO) << "Beam search isn't supported on gpu yet";
/*
paddle::framework::LoDTensor result_ids2, result_scores2;
std::vector<paddle::framework::LoDTensor*> cpu_fetchs2;
cpu_fetchs2.push_back(&result_ids2);
cpu_fetchs2.push_back(&result_scores2);

// Run inference on CUDA GPU
TestInference<paddle::platform::CUDAPlace>(dirname, cpu_feeds, cpu_fetchs2);
LOG(INFO) << result_ids2.lod();
LOG(INFO) << result_ids2.dims();
LOG(INFO) << result_scores2.lod();
LOG(INFO) << result_scores2.dims();

CheckError<int64_t>(result_ids1, result_ids2);
CheckError<float>(result_scores1, result_scores2);
*/
#endif
}
