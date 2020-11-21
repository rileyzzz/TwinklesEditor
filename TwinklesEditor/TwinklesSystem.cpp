#include "TwinklesSystem.h"

TwinklesSystem::TwinklesSystem(const char* filepath)
{
	std::cout << "Loading Twinkles particle system " << filepath << "\n";

	IOArchive Ar(filepath, IODirection::Import);
	if (!Serialize(Ar))
	{
		std::cout << "Particle load failed!\n";
	}
}

void TwinklesSystem::Export(const char* filepath)
{
	std::cout << "Exporting Twinkles particle system " << filepath << "\n";
	IOArchive Ar(filepath, IODirection::Export);
	if (!Serialize(Ar))
	{
		std::cout << "Particle save failed!\n";
	}
}

bool TwinklesSystem::Serialize(IOArchive& Ar)
{
	if (!Ar.ChunkHeader("XFPT")) return false;
	Ar << Version;
	if (Version != 105)
	{
		std::cout << "Unsupported particle version " << Version << "\n";
		return false;
	}

	uint32_t NumFx = Emitters.size();
	Ar << NumFx;
	if(Ar.IsLoading()) Emitters.resize(NumFx);
	for (int Fx = 0; Fx < NumFx; Fx++)
	{
		std::cout << "Emitter " << Fx << "\n";
		Emitter& Emit = Emitters[Fx];
		Emit.Serialize(Ar);
	}

	return true;
}

bool Emitter::Serialize(IOArchive& Ar)
{
	if (!Ar.ChunkHeader("XFTS")) return false;
	Ar << TextureKUID;
	std::cout << "material KUID <kuid:" << TextureKUID.UserID << ":" << TextureKUID.ContentID << ">\n";
	//x, z, y?
	Ar << Position;
	std::cout << "Position: " << Position.DebugString() << "\n";

	Ar << Rotation.x;
	Ar << Rotation.y;
	Ar << Rotation.z;
	Ar << Rotation.w;

	std::cout << "Emitter Size:\n";
	EmitterSize.Serialize(Ar);

	std::cout << "Emission Rate:\n";
	EmissionRate.Serialize(Ar);

	std::cout << "Velocity Cone:\n";
	VelocityCone.Serialize(Ar);

	std::cout << "Z Speed Variance:\n";
	ZSpeedVariance.Serialize(Ar);

	Ar << Type;
	//unknown float value
	float unknown = 1.0f;
	Ar << unknown;

	std::cout << "Lifetime:\n";
	Lifetime.Serialize(Ar);

	std::cout << "Lifetime Variance:\n";
	LifetimeVariance.Serialize(Ar);

	std::cout << "Size Range:\n";
	SizeRange.Serialize(Ar);

	std::cout << "Size Variance:\n";
	SizeVariance.Serialize(Ar);

	std::cout << "Particle Size:\n";
	Size.Serialize(Ar);

	std::cout << "Particle Color:\n";
	Color.Serialize(Ar);

	std::cout << "Max Rotation:\n";
	MaxRotation.Serialize(Ar); //radians

	std::cout << "Gravity:\n";
	Gravity.Serialize(Ar);

	Ar << VelocityMinSpeed;
	Ar << VelocityMaxSpeed;

	std::cout << "Wind Factor:\n";
	WindFactor.Serialize(Ar);

	Ar << VelocityDampening;
	if (!Ar.ChunkHeader("XFDE")) return false;
	return true;
}
