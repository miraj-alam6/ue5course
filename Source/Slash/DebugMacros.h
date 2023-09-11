#pragma once
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Red, true);
#define DRAW_LINE(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f);
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true);
#define DRAW_VECTOR(StartLocation, EndLocation) if (GetWorld()) \
{\
	DRAW_LINE(StartLocation, EndLocation); \
	DRAW_POINT(EndLocation);\
}

#define DRAW_COLOR_SPHERE(StartLocation, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, Color, true)
#define DRAW_BOX_SPAN(StartLocation, EndLocation, BoxLength, BoxWidth) if(GetWorld()) 	DrawDebugBox(GetWorld(), ((StartLocation + EndLocation) / 2.0), FVector((float)FVector::Dist(EndLocation, StartLocation), BoxLength, BoxWidth) / 2.0, UKismetMathLibrary::MakeRotFromX((EndLocation - StartLocation).GetSafeNormal()).Quaternion(), FColor::Red, true, -1, 0, 5.f);