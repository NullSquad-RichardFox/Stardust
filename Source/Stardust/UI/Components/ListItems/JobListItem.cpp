// Fill out your copyright notice in the Description page of Project Settings.


#include "JobListItem.h"
#include "Components/TextBlock.h"
#include "Stardust/Libraries/ListDataLibrary.h"


void UJobListItem::OnItemAddedHandle(UObject* Object)
{
	if (UJobListData* Data = Cast<UJobListData>(Object))
	{
		EJobType JobType;
		int32 TotalJobs, OccupiedJobs;
		Data->GetJobData(JobType, TotalJobs, OccupiedJobs);

		if (UEnum* JobTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EJobType"), true))
		{
			JobName->SetText(JobTypeEnum->GetDisplayNameTextByIndex((int32)JobType));
		}

		WorkedJobsText->SetText(FText::AsNumber(OccupiedJobs));
		TotalJobsText->SetText(FText::AsNumber(TotalJobs));
	}
}