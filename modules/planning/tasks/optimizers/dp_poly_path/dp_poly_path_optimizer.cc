/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#include "modules/planning/tasks/optimizers/dp_poly_path/dp_poly_path_optimizer.h"

//#include "modules/common/util/file.h"
#include "modules/planning/common/planning_gflags.h"
#include "modules/planning/tasks/optimizers/road_graph/dp_road_graph.h"
#include "modules/planning/tasks/optimizers/road_graph/waypoint_sampler.h"

namespace apollo {
namespace planning {

using apollo::common::ErrorCode;
using apollo::common::Status;

DpPolyPathOptimizer::DpPolyPathOptimizer(const TaskConfig &config)
    : PathOptimizer(config) {
  CHECK(config_.has_dp_poly_path_config());
  //SetName("DpPolyPathOptimizer");
}

Status DpPolyPathOptimizer::Process(const SpeedData &speed_data,
                                    const ReferenceLine &,
                                    const common::TrajectoryPoint &init_point,
                                    const bool path_reusable,
                                    PathData *const path_data) {

  AERROR << "...................." << "dp_poly_path used ! " << "\n";                                    
  //检查path_data是否为空                                     
  CHECK_NOTNULL(path_data);
  //读取配置文件，配置文件： modules/planning/conf/planning_config.pb.txt； 对应的proto文件 modules/planning/proto/dp_poly_path_config.proto
  const auto &dp_poly_path_config = config_.dp_poly_path_config();
  //实例化DpRoadGraph，将dp_poly_path_config、reference_line_info、speed_data存入对应的成员变量
  DpRoadGraph dp_road_graph(dp_poly_path_config, reference_line_info_,  speed_data);
  //对应的proto文件是 planning/proto/planning_internal.proto,这个是否是必须的？作用是？
  dp_road_graph.SetDebugLogger(reference_line_info_->mutable_debug());
  dp_road_graph.SetWaypointSampler(new WaypointSampler(dp_poly_path_config.waypoint_sampler_config()));

  if (!dp_road_graph.FindPathTunnel(
          init_point,
          reference_line_info_->path_decision()->obstacles().Items(),
          path_data)) {
    AERROR << "Failed to find tunnel in road graph";
    return Status(ErrorCode::PLANNING_ERROR, "dp_road_graph path generation");
  }

  return Status::OK();
}

}  // namespace planning
}  // namespace apollo
