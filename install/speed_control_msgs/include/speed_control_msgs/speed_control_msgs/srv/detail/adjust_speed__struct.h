// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from speed_control_msgs:srv/AdjustSpeed.idl
// generated code does not contain a copyright notice

#ifndef SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__STRUCT_H_
#define SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/AdjustSpeed in the package speed_control_msgs.
typedef struct speed_control_msgs__srv__AdjustSpeed_Request
{
  /// 方向：1=增加，-1=减少，0=重置
  int8_t direction;
  /// 调整步长（默认0.1）
  double step;
} speed_control_msgs__srv__AdjustSpeed_Request;

// Struct for a sequence of speed_control_msgs__srv__AdjustSpeed_Request.
typedef struct speed_control_msgs__srv__AdjustSpeed_Request__Sequence
{
  speed_control_msgs__srv__AdjustSpeed_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} speed_control_msgs__srv__AdjustSpeed_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/AdjustSpeed in the package speed_control_msgs.
typedef struct speed_control_msgs__srv__AdjustSpeed_Response
{
  bool success;
  rosidl_runtime_c__String message;
  double current_linear;
  double current_angular;
} speed_control_msgs__srv__AdjustSpeed_Response;

// Struct for a sequence of speed_control_msgs__srv__AdjustSpeed_Response.
typedef struct speed_control_msgs__srv__AdjustSpeed_Response__Sequence
{
  speed_control_msgs__srv__AdjustSpeed_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} speed_control_msgs__srv__AdjustSpeed_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__STRUCT_H_
