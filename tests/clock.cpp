#include <engine.hpp>

// Used to rotate the clock arms
Vec3 rotatePointAroundAxisX(Vec3 point, Vec3 pivot, float angle)
{
    // Translate point into origin
	point -= pivot;

	// Rotate
	float newY = point.y * std::cos(angle) - point.z * std::sin(angle);
	float newZ = point.z * std::cos(angle) + point.y * std::sin(angle);

	// Translate back to original place
	return Vec3(point.x, newY, newZ) + pivot;
}

Vec3 rotatePointAroundAxisY(Vec3 point, Vec3 pivot, float angle)
{
    // Translate point into origin
	point -= pivot;

	// Rotate
	float newX = point.x * std::cos(angle) - point.z * std::sin(angle);
	float newZ = point.z * std::cos(angle) + point.x * std::sin(angle);

	// Translate back to original place
	return Vec3(newX, point.y, newZ) + pivot;
}

Vec3 rotatePointAroundAxisZ(Vec3 point, Vec3 pivot, float angle)
{
    // Translate point into origin
	point -= pivot;

	// Rotate
	float newX = point.x * std::cos(angle) - point.y * std::sin(angle);
	float newY = point.y * std::cos(angle) + point.x * std::sin(angle);

	// Translate back to original place
	return Vec3(newX, newY, point.z) + pivot;
}

// Clock scene class
class ClockScene : public Engine3D
{
public:
    // Override methods
    void setup() override;
    void update(float dt) override;
private:
    Mesh floor, secondArm, minuteArm, hourArm;
    // Cylinder resolution (quality)
    int resolution = 36;

    // Cylinder radiuses
    float clockRadius = 20.0f;
    float secondArmRadius = 0.5f;
    float minuteArmRadius = 0.8f;
    float hourArmRadius = 1.0f;

    // Cylinder heights
    float secondArmLength = 12.0f;
    float minuteArmLength = 18.0f;
    float hourArmLength = 9.0f;

    // Arm Y position
    float armPosY = 1.0f;

    // Clock floor height
    float clockHeight = 2.0f;

    // Colors
    SDL_Color clockColor = {255, 255, 255, 255};
    SDL_Color secondArmColor = {255, 0, 0, 255};
    SDL_Color minuteArmColor = {50, 50, 50, 255};
    SDL_Color hourArmColor = {50, 50, 50, 255};

    // Hour mark info
    float hourMarkRadius = .2f;
    float hourMarkLength = 3.0f;
    float hourMarkBorderOffset = 1.0f;
    SDL_Color hourMarkColor = {15, 15, 15, 255};

    // Time info
    float secondArmAngle = M_PIf * 0.1f;
    float minuteArmAngle = M_PIf * 1.3f;
    float hourArmAngle = M_PIf * .2f;
};

void ClockScene::setup()
{
    // Call base class setup
    Engine3D::setup();

    // Set camera pos
    cam.position = {0.0f, 22.0f, 3.0f};
    cam.rotate(-M_PIf * .472f, 0.0f);

    // cam.position = {0.0f, 10.0f, 25.0f};
    // cam.rotate(-M_PIf * .2f, 0.0f);

    // Clock floor
    floor = Mesh::Cylinder(clockRadius, clockHeight, resolution);
    floor.texture.init(clockColor);

    // Seconds arm
    secondArm = Mesh::Cylinder(secondArmRadius, secondArmLength, resolution);
    // Position based on angle
    secondArm.position = rotatePointAroundAxisY(
        Vec3{secondArmLength * .5f, armPosY, 0.0f},
        Vec3{0.0f, armPosY, 0.0f},
        secondArmAngle
    );
    // Rotation
    secondArm.rotation = {
        M_PIf * 0.5f,
        M_PIf * 0.0f,
        secondArmAngle + M_PIf * 0.5f
    };
    secondArm.texture.init(secondArmColor);

    // Minutes arm
    minuteArm = Mesh::Cylinder(minuteArmRadius, minuteArmLength, resolution);
    // Position based on angle
    minuteArm.position = rotatePointAroundAxisY(
        Vec3{minuteArmLength * .5f, armPosY, 0.0f},
        Vec3{0.0f, armPosY, 0.0f},
        minuteArmAngle
    );
    // Rotation
    minuteArm.rotation = {
        M_PIf * 0.5f,
        M_PIf * 0.0f,
        minuteArmAngle + M_PIf * 0.5f
    };
    minuteArm.texture.init(minuteArmColor);

    // Hours arm
    hourArm = Mesh::Cylinder(hourArmRadius, hourArmLength, resolution);
    hourArm.position = rotatePointAroundAxisY(
        Vec3{hourArmLength * .5f, armPosY, 0.0f},
        Vec3{0.0f, armPosY, 0.0f},
        hourArmAngle
    );
    hourArm.rotation = {
        M_PIf * 0.5f,
        M_PIf * 0.0f,
        hourArmAngle + M_PIf * 0.5f
    };
    hourArm.texture.init(hourArmColor);

    // Add clock objects to scene
    addMesh(secondArm);
    addMesh(minuteArm);
    addMesh(hourArm);
    addMesh(floor);

    // Hour marks
    for (int i = 0; i <= 12; i++) {
        // Get angle
        float angle = M_PIf / 6.0f * (float)i;

        // Create mesh
        Mesh mesh = Mesh::Cylinder(hourMarkRadius, hourMarkLength, resolution);

        // Set position
        mesh.position = rotatePointAroundAxisY(
            Vec3{clockRadius - hourMarkLength * 0.5f - hourMarkBorderOffset, armPosY, 0.0f},
            Vec3{0.0f, armPosY, 0.0f},
            angle
        );

        // Set rotation
        mesh.rotation = {
            M_PIf * 0.5f,
            M_PIf * 0.0f,
            angle + M_PIf * 0.5f
        };

        // Set texture
        mesh.texture.init(hourMarkColor);
        addMesh(mesh);
    }

    // Minute marks
    for (int i = 0; i <= 60; i++) {
        // Skip hour marks
        if (i % 5 == 0) continue;

        // Get angle
        float angle = M_PIf / 30.0f * (float)i;

        // Create mesh
        Mesh mesh = Mesh::Cylinder(hourMarkRadius * 0.5f, hourMarkLength * 0.2f, resolution);

        // Set position
        mesh.position = rotatePointAroundAxisY(
            Vec3{clockRadius - hourMarkLength * 0.1f - hourMarkBorderOffset, armPosY, 0.0f},
            Vec3{0.0f, armPosY, 0.0f},
            angle
        );

        // Set rotation
        mesh.rotation = {
            M_PIf * 0.5f,
            M_PIf * 0.0f,
            angle + M_PIf * 0.5f
        };

        // Set texture
        mesh.texture.init(hourMarkColor);
        addMesh(mesh);
    }

    // Add light
    Light light;
    light.direction = {1.5f, -1.0f, 0.0f};
    light.brightness = 1.0f;
    addLight(light);
}

void ClockScene::update(float dt)
{
    // Call base class update
    Engine3D::update(dt);

    // Default camera update
    DefaultCameraMovement(dt);
    DefaultCameraRotation(dt);

    // Close
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        running = false;

    // Time
    int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count() % (3600 * 1000);
    
    float t = static_cast<float>(ms) / 1000.0f;

    // --------------
    // Get clock info

    // Get hours
    float hours = t / 3600.0f;
    hours = hours - 12.0f * std::round(hours / 12.0f);
    
    // Get seconds
    float seconds = t;
    seconds = seconds - 60.0f * std::round(seconds / 60.0f);

    // Get minutes
    float minutes = t / 60.0f;
    minutes = minutes - 60.0f * std::round(minutes / 60.0f);

    // --------------
    // Set clock arms

    // Set second arm
    float secondAngle = std::floor(seconds) / 60.0f * M_PIf * 2.0f;
    // Angle zero is on right, but zero seconds should be on top of clock
    secondAngle -= M_PIf * 0.5f;
    sceneMeshes[0].position = rotatePointAroundAxisY(
        Vec3{secondArmLength * .5f, armPosY, 0.0f},
        Vec3{0.0f, armPosY, 0.0f},
        secondAngle
    );
    sceneMeshes[0].rotation.z = secondAngle + M_PIf * 0.5f;

    // Set minute arm
    float minuteAngle = minutes / 60.0f * M_PIf * 2.0f;
    // Angle zero is on right, but zero minutes should be on top of clock
    minuteAngle -= M_PIf * 0.5f;
    sceneMeshes[1].position = rotatePointAroundAxisY(
        Vec3{minuteArmLength * .5f, armPosY, 0.0f},
        Vec3{0.0f, armPosY, 0.0f},
        minuteAngle
    );
    sceneMeshes[1].rotation.z = minuteAngle + M_PIf * 0.5f;

    // Set hour arm
    float hourAngle = hours / 12.0f * M_PIf * 2.0f;
    // Angle zero is on right, but zero hours should be on top of clock
    hourAngle -= M_PIf * 0.5f;
    sceneMeshes[2].position = rotatePointAroundAxisY(
        Vec3{hourArmLength * .5f, armPosY, 0.0f},
        Vec3{0.0f, armPosY, 0.0f},
        hourAngle
    );
    sceneMeshes[2].rotation.z = hourAngle + M_PIf * 0.5f;


    // Change fov based on scroll
    SetFOV(std::clamp(cam.fov - scroll * 3.f, 5.0f, 160.0f));
}

int main(int argc, char **argv)
{
    // Create and initialize engine
    ClockScene clock;
    if (!clock.init())
    {
        printf("Error initializing engine\n");
        return -1;
    }

    // Run simulation
    clock.run();

    return 0;
}