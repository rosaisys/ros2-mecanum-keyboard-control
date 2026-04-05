// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from speed_control_msgs:srv/AdjustSpeed.idl
// generated code does not contain a copyright notice

#ifndef SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__BUILDER_HPP_
#define SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "speed_control_msgs/srv/detail/adjust_speed__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace speed_control_msgs
{

namespace srv
{

namespace builder
{

class Init_AdjustSpeed_Request_step
{
public:
  explicit Init_AdjustSpeed_Request_step(::speed_control_msgs::srv::AdjustSpeed_Request & msg)
  : msg_(msg)
  {}
  ::speed_control_msgs::srv::AdjustSpeed_Request step(::speed_control_msgs::srv::AdjustSpeed_Request::_step_type arg)
  {
    msg_.step = std::move(arg);
    return std::move(msg_);
  }

private:
  ::speed_control_msgs::srv::AdjustSpeed_Request msg_;
};

class Init_AdjustSpeed_Request_direction
{
public:
  Init_AdjustSpeed_Request_direction()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AdjustSpeed_Request_step direction(::speed_control_msgs::srv::AdjustSpeed_Request::_direction_type arg)
  {
    msg_.direction = std::move(arg);
    return Init_AdjustSpeed_Request_step(msg_);
  }

private:
  ::speed_control_msgs::srv::AdjustSpeed_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::speed_control_msgs::srv::AdjustSpeed_Request>()
{
  return speed_control_msgs::srv::builder::Init_AdjustSpeed_Request_direction();
}

}  // namespace speed_control_msgs


namespace speed_control_msgs
{

namespace srv
{

namespace builder
{

class Init_AdjustSpeed_Response_current_angular
{
public:
  explicit Init_AdjustSpeed_Response_current_angular(::speed_control_msgs::srv::AdjustSpeed_Response & msg)
  : msg_(msg)
  {}
  ::speed_control_msgs::srv::AdjustSpeed_Response current_angular(::speed_control_msgs::srv::AdjustSpeed_Response::_current_angular_type arg)
  {
    msg_.current_angular = std::move(arg);
    return std::move(msg_);
  }

private:
  ::speed_control_msgs::srv::AdjustSpeed_Response msg_;
};

class Init_AdjustSpeed_Response_current_linear
{
public:
  explicit Init_AdjustSpeed_Response_current_linear(::speed_control_msgs::srv::AdjustSpeed_Response & msg)
  : msg_(msg)
  {}
  Init_AdjustSpeed_Response_current_angular current_linear(::speed_control_msgs::srv::AdjustSpeed_Response::_current_linear_type arg)
  {
    msg_.current_linear = std::move(arg);
    return Init_AdjustSpeed_Response_current_angular(msg_);
  }

private:
  ::speed_control_msgs::srv::AdjustSpeed_Response msg_;
};

class Init_AdjustSpeed_Response_message
{
public:
  explicit Init_AdjustSpeed_Response_message(::speed_control_msgs::srv::AdjustSpeed_Response & msg)
  : msg_(msg)
  {}
  Init_AdjustSpeed_Response_current_linear message(::speed_control_msgs::srv::AdjustSpeed_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return Init_AdjustSpeed_Response_current_linear(msg_);
  }

private:
  ::speed_control_msgs::srv::AdjustSpeed_Response msg_;
};

class Init_AdjustSpeed_Response_success
{
public:
  Init_AdjustSpeed_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AdjustSpeed_Response_message success(::speed_control_msgs::srv::AdjustSpeed_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_AdjustSpeed_Response_message(msg_);
  }

private:
  ::speed_control_msgs::srv::AdjustSpeed_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::speed_control_msgs::srv::AdjustSpeed_Response>()
{
  return speed_control_msgs::srv::builder::Init_AdjustSpeed_Response_success();
}

}  // namespace speed_control_msgs

#endif  // SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__BUILDER_HPP_
