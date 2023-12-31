#include <Arduino.h>

#include "motorgo_mini.h"
#include "pid_web_server.h"
#include "trajectory_utils.h"

#define LOOP_HZ 100
#define LOOP_US 1000000 / LOOP_HZ

#define MAX_SCROLL_VELOCITY 50

MotorGo::MotorGoMini* motorgo_mini;
MotorGo::MotorParameters motor_params_ch0;
MotorGo::MotorParameters motor_params_ch1;

waypoint_t waypoints[] = {{0.0, 0.0}, {10.0, 15.5}};
waypoint_t start = {0.0, 0.0};
waypoint_t end = {0.0, 0.5};

waypoint_t* trajectory = nullptr;

// Instantiate the web server
PIDWebServer pid_web_server(8080);

// InlineCurrentSense current_sense = InlineCurrentSense(0.01, 20, 47, 12, _NC);

int cur_waypoint = 0;
int scroll_velocity = 0;

// Joystick data struct
// uint8_t broadcast_address[] = {0x24, 0x0A, 0xC4, 0x00, 0x4C, 0x6E};
// Create struct for data transmission
const int JOYSTICK_DATA_LEN = (6) * sizeof(int);
typedef union
{
  struct __attribute__((packed))
  {
    int joy1_x;
    int joy1_y;
    int joy1_sw;

    int joy2_x;
    int joy2_y;
    int joy2_sw;
  };

  uint8_t raw[JOYSTICK_DATA_LEN];
} joystick_data_t;

int update_waypoint(int cur_waypoint, int joystick_input);

// Callback for receiving joystick data over ESPNow
void on_joystick_data_receive(const uint8_t* mac, const uint8_t* incoming_data,
                              int len);

void on_pid_update(const String& updated_value);
void on_target_waypoint_update(const String& updated_value);

void test();

// Print message at a given frequency, provided in Hz
void freq_print(float freq, const char* message)
{
  static unsigned long last_print_time = 0;
  unsigned long now = millis();

  if (now - last_print_time > 1000.0 / freq)
  {
    Serial.print(message);
    last_print_time = now;
  }
}

void freq_println(float freq, const char* message)
{
  static unsigned long last_print_time = 0;
  unsigned long now = millis();

  if (now - last_print_time > 1000.0 / freq)
  {
    Serial.println(message);
    last_print_time = now;
  }
}

void update_trajectory()
{
  // Check if trajectory exists
  if (trajectory != nullptr)
  {
    // Delete old trajectory
    delete[] trajectory;
  }

  start.q0 = motorgo_mini->get_ch0_position();
  start.q1 = motorgo_mini->get_ch1_position();
  end = waypoints[cur_waypoint];

  int trajectory_len = get_trajectory_len(start, end, 0.1);
  // Allocate memory for trajectory
  trajectory = new waypoint_t[trajectory_len];
  // Interpolate from current position to end position
  //   linear_interpolate(start, end, trajectory, 0.1);

  linear_interpolate(start, end, trajectory, 0.1);
}

void setup()
{
  Serial.begin(115200);

  // Setup the PID web server

  // Callback when session ID is received
  pid_web_server.setSessionIdCallback(
      [](const String& sessionId)
      {
        // Use the received session ID for something, e.g., print it
        Serial.println("Received session ID: " + sessionId);
      });

  // Callback when a new waypoint is set
  pid_web_server.setWaypointCallback(on_target_waypoint_update);

  // Callback when PID parameters are updated
  pid_web_server.setPidCallback(on_pid_update);

  pid_web_server.start();  // Start the web server

  //   // Setup boot pin as input
  //   // Delay program start until boot pin is pulled low
  //   pinMode(0, INPUT);

  //   while (digitalRead(0) == HIGH)
  //   {
  //     delay(100);
  //   }

  //   Serial.println("Starting program");

  //   // Setup motor parameters
  motor_params_ch0.pole_pairs = 11;
  motor_params_ch0.power_supply_voltage = 5.0;
  motor_params_ch0.voltage_limit = 5.0;
  motor_params_ch0.current_limit = 320;
  motor_params_ch0.velocity_limit = 100.0;
  motor_params_ch0.calibration_voltage = 3.0;

  motor_params_ch1.pole_pairs = 11;
  motor_params_ch1.power_supply_voltage = 5.0;
  motor_params_ch1.voltage_limit = 5.0;
  motor_params_ch1.current_limit = 320;
  motor_params_ch1.velocity_limit = 100.0;
  motor_params_ch1.calibration_voltage = 3.0;

  // Instantiate motorgo mini board
  motorgo_mini = new MotorGo::MotorGoMini();

  // Setup both channels
  motorgo_mini->init_ch0(motor_params_ch0, false, false);
  motorgo_mini->init_ch1(motor_params_ch1, false, false);

  // Set position controller parameters
  motorgo_mini->set_velocity_controller_ch0(
      pid_web_server.get_velocity_pid_params_ch0());
  motorgo_mini->set_velocity_controller_ch1(
      pid_web_server.get_velocity_pid_params_ch1());

  motorgo_mini->set_position_controller_ch0(
      pid_web_server.get_position_pid_params_ch0());
  motorgo_mini->set_position_controller_ch1(
      pid_web_server.get_position_pid_params_ch1());

  // Set closed-loop velocity control mode
  motorgo_mini->set_control_mode_ch0(MotorGo::ControlMode::Position);
  motorgo_mini->set_control_mode_ch1(MotorGo::ControlMode::Position);

  motorgo_mini->zero_position_ch0();
  motorgo_mini->zero_position_ch1();

  // Allocate memory for trajectory
  //   trajectory = new waypoint_t[trajectory_len];
  //   // Define array for trajectory

  //   // Interpolate from current position to end position
  //   //   linear_interpolate(start, end, trajectory, 0.1);
  //   start.q0 = motorgo_mini->get_ch0_position();
  //   start.q1 = motorgo_mini->get_ch1_position();
  //   end = waypoints[cur_waypoint];
  //   linear_interpolate(start, end, trajectory, 0.1);

  update_trajectory();

  delay(1000);

  //   //   Enable motors
  motorgo_mini->enable_ch0();
  //   motorgo_mini->enable_ch1();
}

int i = 0;
// Constrain loop speed to 250 Hz
unsigned long last_loop_time = 0;
float vel = 30;
void loop()
{
  test();
  // Loop Motors
  motorgo_mini->loop_ch0();
  motorgo_mini->loop_ch1();

  // Spin forward
  motorgo_mini->set_target_position_ch0(trajectory[cur_waypoint].q0);
  motorgo_mini->set_target_position_ch1(trajectory[cur_waypoint].q1);

  //   // print velocity for both channels
  //   char str_buffer[100];
  //   sprintf(str_buffer, "Ch0: %f, Ch1: %f\n",
  //   motorgo_mini->get_ch0_velocity(),
  //           motorgo_mini->get_ch1_velocity());

  //   freq_print(10, str_buffer);

  //   //   vel += 0.01;
  //   //   if (vel > 5)
  //   //   {
  //   //     vel = 0;
  //   //   }

  // Print ch0 position, and desired position
  char str_buffer[200];
  sprintf(str_buffer, "Ch0: %f, CTarget: %f\n",
          motorgo_mini->get_ch0_position(), trajectory[cur_waypoint].q0);

  freq_print(10, str_buffer);

  //   Update current waypoint from joystick
  // TODO: Read joystick data

  // Print cur waypoint + scroll velocity
  cur_waypoint = update_waypoint(cur_waypoint, scroll_velocity);

  // Delay necessary amount micros
  unsigned long now = micros();
  unsigned long loop_time = now - last_loop_time;

  if (loop_time < LOOP_US)
  {
    delayMicroseconds(LOOP_US - loop_time);

    // Serial.print("Loop time: ");
  }

  last_loop_time = now;
}

// Function definitions
void on_joystick_data_receive(const uint8_t* mac, const uint8_t* incoming_data,
                              int len)
{
  joystick_data_t data;
  memcpy(&data.raw, incoming_data, JOYSTICK_DATA_LEN);

  //   scroll_velocity = MAX_SCROLL_VELOCITY * ((float)data.joy2_x / 100.0f);
  scroll_velocity = MAX_SCROLL_VELOCITY * ((float)data.joy2_x / 100.0f);
}

void test() { scroll_velocity = MAX_SCROLL_VELOCITY * ((float)75 / 100.0f); }

int update_waypoint(int cur_waypoint, int scroll_velocity)
{
  static unsigned long last_update_time = millis();

  unsigned long now = millis();

  // Compute delta waypoint
  int delta_waypoint = scroll_velocity * (now - last_update_time) / 1000.0;

  // Only update last update time if delta waypoint is non-zero
  if (delta_waypoint != 0)
  {
    last_update_time = now;
  }

  int new_waypoint = cur_waypoint + delta_waypoint;
  if (new_waypoint < 0)
  {
    new_waypoint = 0;
  }
  else if (new_waypoint >= get_trajectory_len(start, end, 0.1))
  {
    new_waypoint = get_trajectory_len(start, end, 0.1) - 1;
  }

  return new_waypoint;
}

void on_pid_update(const String& updated_value)
{
  Serial.println("PID Update");
  // Set all PID Parameters
  motorgo_mini->set_velocity_controller_ch0(
      pid_web_server.get_velocity_pid_params_ch0());
  motorgo_mini->set_velocity_controller_ch1(
      pid_web_server.get_velocity_pid_params_ch1());

  motorgo_mini->set_position_controller_ch0(
      pid_web_server.get_position_pid_params_ch0());
  motorgo_mini->set_position_controller_ch1(
      pid_web_server.get_position_pid_params_ch1());

  motorgo_mini->disable_ch1();
}

void on_target_waypoint_update(const String& updated_value)
{
  cur_waypoint = pid_web_server.get_target_waypoint();
  Serial.print("Target waypoint: ");
  Serial.println(cur_waypoint);
  update_trajectory();
}