// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from speed_control_msgs:srv/AdjustSpeed.idl
// generated code does not contain a copyright notice
#include "speed_control_msgs/srv/detail/adjust_speed__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
speed_control_msgs__srv__AdjustSpeed_Request__init(speed_control_msgs__srv__AdjustSpeed_Request * msg)
{
  if (!msg) {
    return false;
  }
  // direction
  // step
  return true;
}

void
speed_control_msgs__srv__AdjustSpeed_Request__fini(speed_control_msgs__srv__AdjustSpeed_Request * msg)
{
  if (!msg) {
    return;
  }
  // direction
  // step
}

bool
speed_control_msgs__srv__AdjustSpeed_Request__are_equal(const speed_control_msgs__srv__AdjustSpeed_Request * lhs, const speed_control_msgs__srv__AdjustSpeed_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // direction
  if (lhs->direction != rhs->direction) {
    return false;
  }
  // step
  if (lhs->step != rhs->step) {
    return false;
  }
  return true;
}

bool
speed_control_msgs__srv__AdjustSpeed_Request__copy(
  const speed_control_msgs__srv__AdjustSpeed_Request * input,
  speed_control_msgs__srv__AdjustSpeed_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // direction
  output->direction = input->direction;
  // step
  output->step = input->step;
  return true;
}

speed_control_msgs__srv__AdjustSpeed_Request *
speed_control_msgs__srv__AdjustSpeed_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  speed_control_msgs__srv__AdjustSpeed_Request * msg = (speed_control_msgs__srv__AdjustSpeed_Request *)allocator.allocate(sizeof(speed_control_msgs__srv__AdjustSpeed_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(speed_control_msgs__srv__AdjustSpeed_Request));
  bool success = speed_control_msgs__srv__AdjustSpeed_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
speed_control_msgs__srv__AdjustSpeed_Request__destroy(speed_control_msgs__srv__AdjustSpeed_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    speed_control_msgs__srv__AdjustSpeed_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
speed_control_msgs__srv__AdjustSpeed_Request__Sequence__init(speed_control_msgs__srv__AdjustSpeed_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  speed_control_msgs__srv__AdjustSpeed_Request * data = NULL;

  if (size) {
    data = (speed_control_msgs__srv__AdjustSpeed_Request *)allocator.zero_allocate(size, sizeof(speed_control_msgs__srv__AdjustSpeed_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = speed_control_msgs__srv__AdjustSpeed_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        speed_control_msgs__srv__AdjustSpeed_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
speed_control_msgs__srv__AdjustSpeed_Request__Sequence__fini(speed_control_msgs__srv__AdjustSpeed_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      speed_control_msgs__srv__AdjustSpeed_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

speed_control_msgs__srv__AdjustSpeed_Request__Sequence *
speed_control_msgs__srv__AdjustSpeed_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  speed_control_msgs__srv__AdjustSpeed_Request__Sequence * array = (speed_control_msgs__srv__AdjustSpeed_Request__Sequence *)allocator.allocate(sizeof(speed_control_msgs__srv__AdjustSpeed_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = speed_control_msgs__srv__AdjustSpeed_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
speed_control_msgs__srv__AdjustSpeed_Request__Sequence__destroy(speed_control_msgs__srv__AdjustSpeed_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    speed_control_msgs__srv__AdjustSpeed_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
speed_control_msgs__srv__AdjustSpeed_Request__Sequence__are_equal(const speed_control_msgs__srv__AdjustSpeed_Request__Sequence * lhs, const speed_control_msgs__srv__AdjustSpeed_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!speed_control_msgs__srv__AdjustSpeed_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
speed_control_msgs__srv__AdjustSpeed_Request__Sequence__copy(
  const speed_control_msgs__srv__AdjustSpeed_Request__Sequence * input,
  speed_control_msgs__srv__AdjustSpeed_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(speed_control_msgs__srv__AdjustSpeed_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    speed_control_msgs__srv__AdjustSpeed_Request * data =
      (speed_control_msgs__srv__AdjustSpeed_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!speed_control_msgs__srv__AdjustSpeed_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          speed_control_msgs__srv__AdjustSpeed_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!speed_control_msgs__srv__AdjustSpeed_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `message`
#include "rosidl_runtime_c/string_functions.h"

bool
speed_control_msgs__srv__AdjustSpeed_Response__init(speed_control_msgs__srv__AdjustSpeed_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // message
  if (!rosidl_runtime_c__String__init(&msg->message)) {
    speed_control_msgs__srv__AdjustSpeed_Response__fini(msg);
    return false;
  }
  // current_linear
  // current_angular
  return true;
}

void
speed_control_msgs__srv__AdjustSpeed_Response__fini(speed_control_msgs__srv__AdjustSpeed_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // message
  rosidl_runtime_c__String__fini(&msg->message);
  // current_linear
  // current_angular
}

bool
speed_control_msgs__srv__AdjustSpeed_Response__are_equal(const speed_control_msgs__srv__AdjustSpeed_Response * lhs, const speed_control_msgs__srv__AdjustSpeed_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->message), &(rhs->message)))
  {
    return false;
  }
  // current_linear
  if (lhs->current_linear != rhs->current_linear) {
    return false;
  }
  // current_angular
  if (lhs->current_angular != rhs->current_angular) {
    return false;
  }
  return true;
}

bool
speed_control_msgs__srv__AdjustSpeed_Response__copy(
  const speed_control_msgs__srv__AdjustSpeed_Response * input,
  speed_control_msgs__srv__AdjustSpeed_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // message
  if (!rosidl_runtime_c__String__copy(
      &(input->message), &(output->message)))
  {
    return false;
  }
  // current_linear
  output->current_linear = input->current_linear;
  // current_angular
  output->current_angular = input->current_angular;
  return true;
}

speed_control_msgs__srv__AdjustSpeed_Response *
speed_control_msgs__srv__AdjustSpeed_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  speed_control_msgs__srv__AdjustSpeed_Response * msg = (speed_control_msgs__srv__AdjustSpeed_Response *)allocator.allocate(sizeof(speed_control_msgs__srv__AdjustSpeed_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(speed_control_msgs__srv__AdjustSpeed_Response));
  bool success = speed_control_msgs__srv__AdjustSpeed_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
speed_control_msgs__srv__AdjustSpeed_Response__destroy(speed_control_msgs__srv__AdjustSpeed_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    speed_control_msgs__srv__AdjustSpeed_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
speed_control_msgs__srv__AdjustSpeed_Response__Sequence__init(speed_control_msgs__srv__AdjustSpeed_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  speed_control_msgs__srv__AdjustSpeed_Response * data = NULL;

  if (size) {
    data = (speed_control_msgs__srv__AdjustSpeed_Response *)allocator.zero_allocate(size, sizeof(speed_control_msgs__srv__AdjustSpeed_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = speed_control_msgs__srv__AdjustSpeed_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        speed_control_msgs__srv__AdjustSpeed_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
speed_control_msgs__srv__AdjustSpeed_Response__Sequence__fini(speed_control_msgs__srv__AdjustSpeed_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      speed_control_msgs__srv__AdjustSpeed_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

speed_control_msgs__srv__AdjustSpeed_Response__Sequence *
speed_control_msgs__srv__AdjustSpeed_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  speed_control_msgs__srv__AdjustSpeed_Response__Sequence * array = (speed_control_msgs__srv__AdjustSpeed_Response__Sequence *)allocator.allocate(sizeof(speed_control_msgs__srv__AdjustSpeed_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = speed_control_msgs__srv__AdjustSpeed_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
speed_control_msgs__srv__AdjustSpeed_Response__Sequence__destroy(speed_control_msgs__srv__AdjustSpeed_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    speed_control_msgs__srv__AdjustSpeed_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
speed_control_msgs__srv__AdjustSpeed_Response__Sequence__are_equal(const speed_control_msgs__srv__AdjustSpeed_Response__Sequence * lhs, const speed_control_msgs__srv__AdjustSpeed_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!speed_control_msgs__srv__AdjustSpeed_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
speed_control_msgs__srv__AdjustSpeed_Response__Sequence__copy(
  const speed_control_msgs__srv__AdjustSpeed_Response__Sequence * input,
  speed_control_msgs__srv__AdjustSpeed_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(speed_control_msgs__srv__AdjustSpeed_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    speed_control_msgs__srv__AdjustSpeed_Response * data =
      (speed_control_msgs__srv__AdjustSpeed_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!speed_control_msgs__srv__AdjustSpeed_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          speed_control_msgs__srv__AdjustSpeed_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!speed_control_msgs__srv__AdjustSpeed_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
