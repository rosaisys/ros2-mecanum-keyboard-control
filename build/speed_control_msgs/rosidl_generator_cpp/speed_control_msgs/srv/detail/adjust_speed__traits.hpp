// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from speed_control_msgs:srv/AdjustSpeed.idl
// generated code does not contain a copyright notice

#ifndef SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__TRAITS_HPP_
#define SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "speed_control_msgs/srv/detail/adjust_speed__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace speed_control_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const AdjustSpeed_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: direction
  {
    out << "direction: ";
    rosidl_generator_traits::value_to_yaml(msg.direction, out);
    out << ", ";
  }

  // member: step
  {
    out << "step: ";
    rosidl_generator_traits::value_to_yaml(msg.step, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const AdjustSpeed_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: direction
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "direction: ";
    rosidl_generator_traits::value_to_yaml(msg.direction, out);
    out << "\n";
  }

  // member: step
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "step: ";
    rosidl_generator_traits::value_to_yaml(msg.step, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const AdjustSpeed_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace speed_control_msgs

namespace rosidl_generator_traits
{

[[deprecated("use speed_control_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const speed_control_msgs::srv::AdjustSpeed_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  speed_control_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use speed_control_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const speed_control_msgs::srv::AdjustSpeed_Request & msg)
{
  return speed_control_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<speed_control_msgs::srv::AdjustSpeed_Request>()
{
  return "speed_control_msgs::srv::AdjustSpeed_Request";
}

template<>
inline const char * name<speed_control_msgs::srv::AdjustSpeed_Request>()
{
  return "speed_control_msgs/srv/AdjustSpeed_Request";
}

template<>
struct has_fixed_size<speed_control_msgs::srv::AdjustSpeed_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<speed_control_msgs::srv::AdjustSpeed_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<speed_control_msgs::srv::AdjustSpeed_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace speed_control_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const AdjustSpeed_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << ", ";
  }

  // member: message
  {
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << ", ";
  }

  // member: current_linear
  {
    out << "current_linear: ";
    rosidl_generator_traits::value_to_yaml(msg.current_linear, out);
    out << ", ";
  }

  // member: current_angular
  {
    out << "current_angular: ";
    rosidl_generator_traits::value_to_yaml(msg.current_angular, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const AdjustSpeed_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }

  // member: message
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << "\n";
  }

  // member: current_linear
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "current_linear: ";
    rosidl_generator_traits::value_to_yaml(msg.current_linear, out);
    out << "\n";
  }

  // member: current_angular
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "current_angular: ";
    rosidl_generator_traits::value_to_yaml(msg.current_angular, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const AdjustSpeed_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace speed_control_msgs

namespace rosidl_generator_traits
{

[[deprecated("use speed_control_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const speed_control_msgs::srv::AdjustSpeed_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  speed_control_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use speed_control_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const speed_control_msgs::srv::AdjustSpeed_Response & msg)
{
  return speed_control_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<speed_control_msgs::srv::AdjustSpeed_Response>()
{
  return "speed_control_msgs::srv::AdjustSpeed_Response";
}

template<>
inline const char * name<speed_control_msgs::srv::AdjustSpeed_Response>()
{
  return "speed_control_msgs/srv/AdjustSpeed_Response";
}

template<>
struct has_fixed_size<speed_control_msgs::srv::AdjustSpeed_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<speed_control_msgs::srv::AdjustSpeed_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<speed_control_msgs::srv::AdjustSpeed_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<speed_control_msgs::srv::AdjustSpeed>()
{
  return "speed_control_msgs::srv::AdjustSpeed";
}

template<>
inline const char * name<speed_control_msgs::srv::AdjustSpeed>()
{
  return "speed_control_msgs/srv/AdjustSpeed";
}

template<>
struct has_fixed_size<speed_control_msgs::srv::AdjustSpeed>
  : std::integral_constant<
    bool,
    has_fixed_size<speed_control_msgs::srv::AdjustSpeed_Request>::value &&
    has_fixed_size<speed_control_msgs::srv::AdjustSpeed_Response>::value
  >
{
};

template<>
struct has_bounded_size<speed_control_msgs::srv::AdjustSpeed>
  : std::integral_constant<
    bool,
    has_bounded_size<speed_control_msgs::srv::AdjustSpeed_Request>::value &&
    has_bounded_size<speed_control_msgs::srv::AdjustSpeed_Response>::value
  >
{
};

template<>
struct is_service<speed_control_msgs::srv::AdjustSpeed>
  : std::true_type
{
};

template<>
struct is_service_request<speed_control_msgs::srv::AdjustSpeed_Request>
  : std::true_type
{
};

template<>
struct is_service_response<speed_control_msgs::srv::AdjustSpeed_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__TRAITS_HPP_
