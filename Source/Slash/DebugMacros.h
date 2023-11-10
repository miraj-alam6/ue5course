#pragma once
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

//SHAPES
#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Red, true);
//This just says color in the name but it also makes the sphere smaller than above and lasts only 5 seconds
#define DRAW_SPHERE_COLOR(Location, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 12.f, 12, Color, false, 5.0f);
#define DRAW_SPHERE_SingleFrame(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Red, false, -1.0f);
#define DRAW_LINE(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f);
#define DRAW_LINE_SingleFrame(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.0f, 0, 1.f);
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true);
#define DRAW_POINT_SingleFrame(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, false, -1.0f);
#define DRAW_VECTOR(StartLocation, EndLocation) if (GetWorld()) \
{\
	DRAW_LINE(StartLocation, EndLocation); \
	DRAW_POINT(EndLocation);\
}
#define DRAW_VECTOR_SingleFrame(StartLocation, EndLocation) if (GetWorld()) \
{\
	DRAW_LINE_SingleFrame(StartLocation, EndLocation); \
	DRAW_POINT_SingleFrame(EndLocation);\
}

#define DRAW_COLOR_SPHERE(StartLocation, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, Color, true)
#define DRAW_BOX_SPAN(StartLocation, EndLocation, BoxLength, BoxWidth) if(GetWorld()) 	DrawDebugBox(GetWorld(), ((StartLocation + EndLocation) / 2.0), FVector((float)FVector::Dist(EndLocation, StartLocation), BoxLength, BoxWidth) / 2.0, UKismetMathLibrary::MakeRotFromX((EndLocation - StartLocation).GetSafeNormal()).Quaternion(), FColor::Red, true, -1, 0, 5.f);

//LOGS
#define SIMPLE_WARNING(CString) UE_LOG(LogTemp, Warning, TEXT(CString));
