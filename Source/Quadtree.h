// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

/**
 * Quadtree:
 * Spatial partitioning data structure for 2D space.
 * 
 * Time Complexity:
 * - Insert: O(log n) average
 * - Query: O(log n + k) where k is results
 * 
 * Space Complexity: O(n)
 * 
 * Use Case: Spatial queries for enemies, collision detection, nearest enemy finding
 */

struct FQuadtreePoint
{
    FVector2D Position;
    AActor* Data;

    FQuadtreePoint() : Data(nullptr) {}
    FQuadtreePoint(const FVector2D& InPos, AActor* InData)
        : Position(InPos), Data(InData)
    {
    }
};

struct FQuadtreeBounds
{
    FVector2D Center;
    FVector2D HalfSize;

    FQuadtreeBounds() {}
    FQuadtreeBounds(const FVector2D& InCenter, const FVector2D& InHalfSize)
        : Center(InCenter), HalfSize(InHalfSize)
    {
    }

    bool Contains(const FVector2D& Point) const
    {
        return (Point.X >= Center.X - HalfSize.X &&
                Point.X <= Center.X + HalfSize.X &&
                Point.Y >= Center.Y - HalfSize.Y &&
                Point.Y <= Center.Y + HalfSize.Y);
    }

    bool Intersects(const FQuadtreeBounds& Other) const
    {
        return !(Other.Center.X - Other.HalfSize.X > Center.X + HalfSize.X ||
                 Other.Center.X + Other.HalfSize.X < Center.X - HalfSize.X ||
                 Other.Center.Y - Other.HalfSize.Y > Center.Y + HalfSize.Y ||
                 Other.Center.Y + Other.HalfSize.Y < Center.Y - HalfSize.Y);
    }
};

class PROJECT_GOLDFISH_API FQuadtree
{
private:
    static const int32 MAX_CAPACITY = 4;
    static const int32 MAX_DEPTH = 8;

    FQuadtreeBounds Boundary;
    TArray<FQuadtreePoint> Points;
    int32 CurrentDepth;

    TSharedPtr<FQuadtree> NorthWest;
    TSharedPtr<FQuadtree> NorthEast;
    TSharedPtr<FQuadtree> SouthWest;
    TSharedPtr<FQuadtree> SouthEast;

    bool bSubdivided;

    void Subdivide()
    {
        if (bSubdivided || CurrentDepth >= MAX_DEPTH)
        {
            return;
        }

        FVector2D QuarterSize = Boundary.HalfSize * 0.5f;

        FVector2D NWCenter(Boundary.Center.X - QuarterSize.X, Boundary.Center.Y + QuarterSize.Y);
        NorthWest = MakeShared<FQuadtree>(FQuadtreeBounds(NWCenter, QuarterSize), CurrentDepth + 1);

        FVector2D NECenter(Boundary.Center.X + QuarterSize.X, Boundary.Center.Y + QuarterSize.Y);
        NorthEast = MakeShared<FQuadtree>(FQuadtreeBounds(NECenter, QuarterSize), CurrentDepth + 1);

        FVector2D SWCenter(Boundary.Center.X - QuarterSize.X, Boundary.Center.Y - QuarterSize.Y);
        SouthWest = MakeShared<FQuadtree>(FQuadtreeBounds(SWCenter, QuarterSize), CurrentDepth + 1);

        FVector2D SECenter(Boundary.Center.X + QuarterSize.X, Boundary.Center.Y - QuarterSize.Y);
        SouthEast = MakeShared<FQuadtree>(FQuadtreeBounds(SECenter, QuarterSize), CurrentDepth + 1);

        bSubdivided = true;

        TArray<FQuadtreePoint> PointsCopy = Points;
        Points.Empty();

        for (const FQuadtreePoint& Point : PointsCopy)
        {
            InsertIntoChildren(Point);
        }
    }

    bool InsertIntoChildren(const FQuadtreePoint& Point)
    {
        if (NorthWest->Insert(Point)) return true;
        if (NorthEast->Insert(Point)) return true;
        if (SouthWest->Insert(Point)) return true;
        if (SouthEast->Insert(Point)) return true;
        return false;
    }

public:
    FQuadtree(const FQuadtreeBounds& InBoundary, int32 Depth = 0)
        : Boundary(InBoundary)
        , CurrentDepth(Depth)
        , bSubdivided(false)
    {
        Points.Reserve(MAX_CAPACITY);
    }

    bool Insert(const FQuadtreePoint& Point)
    {
        if (!Boundary.Contains(Point.Position))
        {
            return false;
        }

        if (!bSubdivided && Points.Num() < MAX_CAPACITY)
        {
            Points.Add(Point);
            return true;
        }

        if (!bSubdivided)
        {
            Subdivide();
        }

        return InsertIntoChildren(Point);
    }

    void Query(const FQuadtreeBounds& Range, TArray<FQuadtreePoint>& OutPoints) const
    {
        if (!Boundary.Intersects(Range))
        {
            return;
        }

        for (const FQuadtreePoint& Point : Points)
        {
            if (Range.Contains(Point.Position))
            {
                OutPoints.Add(Point);
            }
        }

        if (bSubdivided)
        {
            NorthWest->Query(Range, OutPoints);
            NorthEast->Query(Range, OutPoints);
            SouthWest->Query(Range, OutPoints);
            SouthEast->Query(Range, OutPoints);
        }
    }

    void QueryRadius(const FVector2D& Center, float Radius, TArray<FQuadtreePoint>& OutPoints) const
    {
        FQuadtreeBounds Range(Center, FVector2D(Radius, Radius));
        TArray<FQuadtreePoint> CandidatePoints;
        Query(Range, CandidatePoints);

        float RadiusSquared = Radius * Radius;
        for (const FQuadtreePoint& Point : CandidatePoints)
        {
            float DistSquared = FVector2D::DistSquared(Point.Position, Center);
            if (DistSquared <= RadiusSquared)
            {
                OutPoints.Add(Point);
            }
        }
    }

    void Clear()
    {
        Points.Empty();
        
        if (bSubdivided)
        {
            NorthWest->Clear();
            NorthEast->Clear();
            SouthWest->Clear();
            SouthEast->Clear();
            
            NorthWest.Reset();
            NorthEast.Reset();
            SouthWest.Reset();
            SouthEast.Reset();
            
            bSubdivided = false;
        }
    }

    int32 GetSize() const
    {
        int32 Total = Points.Num();
        
        if (bSubdivided)
        {
            Total += NorthWest->GetSize();
            Total += NorthEast->GetSize();
            Total += SouthWest->GetSize();
            Total += SouthEast->GetSize();
        }
        
        return Total;
    }

    bool IsSubdivided() const { return bSubdivided; }
};