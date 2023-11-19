# RancUtilities Plugin

## Overview

RancUtilities is a specialized plugin designed for Unreal Engine projects. It provides a suite of utility functions primarily focused on sorting and managing arrays of objects within the Unreal Engine environment.

## Features

+ Sortable Interface Implementation: The plugin introduces an interface ISortableElement that allows objects to be compared and sorted. This interface is particularly useful for creating custom sorting logic for UObject-derived classes.

+ Sorting Library: A core feature of the plugin is the RancSortingLibrary, which includes functions to sort arrays of objects implementing the ISortableElement interface. It offers both in-place sorting and returning a sorted copy of the array.

+ Blueprint functions: 
	ForceDestroyComponent: for destroying components on other actors from blueprints (default destroy component does not work outside owning actor)

+ TArray Wrapper: Provide a standardized way to store TArray within other data structures such as TMap

## Usage

The plugin's functions are designed to be intuitive for developers familiar with Unreal Engine and C++. Objects that need to be sorted should implement the ISortableElement interface. The sorting and utility functions can then be used in C++ code or exposed to Blueprints as needed.
Integration

The plugin is easily integrable into existing Unreal Engine projects and requires minimal setup. Simply include the necessary headers and start utilizing the provided functionalities in your project.