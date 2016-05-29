#!/bin/bash
# copy binaries to release folder

jgit=..
jplatform=android
mkdir -p $jgit/release/$jplatform/j32/x86
mkdir -p $jgit/release/$jplatform/j32/armeabi
mkdir -p $jgit/release/$jplatform/j32/armeabi-v7a
rm -f $jgit/release/$jplatform/j32/x86/*
rm -f $jgit/release/$jplatform/j32/armeabi/*
rm -f $jgit/release/$jplatform/j32/armeabi-v7a/*

cp $jgit/android/libs/x86/*         $jgit/release/$jplatform/j32/x86/.
cp $jgit/android/libs/armeabi/*     $jgit/release/$jplatform/j32/armeabi/.
cp $jgit/android/libs/armeabi-v7a/* $jgit/release/$jplatform/j32/armeabi-v7a/.
