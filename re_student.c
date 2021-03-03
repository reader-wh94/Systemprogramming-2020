//***************************************************************
//                   HEADER FILE USED IN PROJECT
//***************************************************************

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "getch.h"

//***************************************************************
//                   STURUCTURE USED IN PROJECT
//****************************************************************

struct student
{
	int rollno;
	char name[50];
	int p_marks, c_marks;
	double per;
	char grade;
}st;

//***************************************************************
//        global declaration
//****************************************************************

FILE *fptr;

//***************************************************************
//        function to write in file
//****************************************************************

void write_student()
{	
	int fd;
	fd = open("student.dat", O_CREAT | O_WRONLY, 0666);	
	while(1)
{	
	printf("\nPlease Enter The New Details of student \n");
	printf("\nEnter The roll number of student(exit: 0) ");
	scanf("%d", &st.rollno);
	getchar(); // flushing buffer
	if(st.rollno == 0) break;	
	printf("\n\nEnter The Name of student ");
	fgets(st.name, sizeof(st.name), stdin);
	printf("\nEnter The marks in physics out of 100 : ");
	scanf("%d", &st.p_marks);
	printf("\nEnter The marks in chemistry out of 100 : ");
	scanf("%d", &st.c_marks);

	st.per = (st.p_marks + st.c_marks) / 2.0;
	if (st.per >= 60)
		st.grade = 'A';
	else if (st.per >= 50 && st.per<60)
		st.grade = 'B';
	else if (st.per >= 33 && st.per<50)
		st.grade = 'C';
	else
		st.grade = 'F';

	write(fd, &st, sizeof(st));
}
	close(fd);
	printf("\n\nStudent Record Has Been Created.  Press any key.... ");
	getch();
}


//***************************************************************
//        function to read all records from file
//****************************************************************


void display_all()
{
	//errno_t err;
	getchar();
	int i, fd;
	system("clear");
	printf("\n\n\n\t\tDISPLAY ALL RECORD !!!\n\n");
	printf("====================================================\n");
	printf("R.No.  Name       P   C   Ave   Grade\n");
	printf("====================================================\n");
	
	fd = open("student.dat", O_RDONLY, 0666);		
	if (fd == -1)
		return;

	while ( (i = read(fd, &st, sizeof(st))) > 0)
	{
		printf("%-6d %-10s %-3d %-3d %-3.2f  %-1c\n", 
			st.rollno, st.name, st.p_marks, st.c_marks, st.per, st.grade);
	}
	close(fd);
	getch();
}


//***************************************************************
//        function to read specific record from file
//****************************************************************


void display_sp(int n)
{
	int fd, flag = 0;	
	fd = open("student.dat", O_RDWR, 0666);	
	if (fd == -1)
		return;
	//while ((read(&st, sizeof(st), 1, fptr))>0)
	while ((read(fd, &st, sizeof(st)))>0)
	{
		if (st.rollno == n)
		{
			system("clear");
			printf("\nRoll number of student : %d", st.rollno);
			printf("\nName of student : %s", st.name);
			printf("\nMarks in Physics : %d", st.p_marks);
			printf("\nMarks in Chemistry : %d", st.c_marks);
			printf("\nPercentage of student is  : %.2f", st.per);
			printf("\nGrade of student is : %c", st.grade);
			flag = 1;
		}
	}
	close(fd);
	if (flag == 0)
		printf("\n\nrecord not exist");
	getch();
}


//***************************************************************
//        function to modify record of file
//****************************************************************


void modify_student()
{
	int no, found = 0, i, fd;
	system("clear");
	printf("\n\n\tTo Modify ");
	printf("\n\n\tPlease Enter The roll number of student");
	scanf("%d", &no);	
	fd = open("student.dat", O_RDWR, 0666);	
	if (fd == -1)
		return;

	while ((i = read(fd, &st, sizeof(st))) > 0 && found == 0)
	{
		if (st.rollno == no)
		{
			printf("\nRoll number of student : %d", st.rollno);
			printf("\nName of student : %s", st.name);
			printf("\nMarks in Physics : %d", st.p_marks);
			printf("\nMarks in Chemistry : %d", st.c_marks);
			printf("\nPercentage of student is  : %.2f", st.per);
			printf("\nGrade of student is : %c", st.grade);
			printf("\nPlease Enter The New Details of student \n");
			printf("\nEnter The roll number of student ");
			scanf("%d", &st.rollno);
			getchar();  //flushing buffer (fflsh는 작동안함)
			printf("\n\nEnter The Name of student ");
			fgets(st.name, sizeof(st.name), stdin);			
			printf("\nEnter The marks in physics out of 100 : ");
			scanf("%d", &st.p_marks);
			printf("\nEnter The marks in chemistry out of 100 : ");
			scanf("%d", &st.c_marks);

			st.per = (st.p_marks + st.c_marks) / 2.0;
			if (st.per >= 60)
				st.grade = 'A';
			else if (st.per >= 50 && st.per<60)
				st.grade = 'B';
			else if (st.per >= 33 && st.per<50)
				st.grade = 'C';
			else
				st.grade = 'F';


			lseek(fd, -(int)sizeof(st), 1);  // SEEK_CUR

			write(fd, &st, sizeof(st));

			printf("\n\n\t Record Updated");

			found = 1;
			break;
		}
	}

	close(fd);
	if (found == 0)
		printf("\n\n Record Not Found ");
	getch();
}


//***************************************************************
//        function to delete record of file
//****************************************************************


void delete_student()
{
	int no,fd,fd2;
	system("clear");
	printf("\n\n\n\tDelete Record");
	printf("\n\nPlease Enter The roll number of student You Want To Delete");
	scanf("%d", &no);

	fd = open("student.dat", O_RDWR, 0666);	
	if (fd == -1)
		return;

	fd2 = open("Temp.dat", O_CREAT | O_WRONLY, 0666);	
	//fd = 0; // move file pointer to 0
	lseek(fd, 0, SEEK_SET);
	
	while ((read(fd, &st, sizeof(st))) > 0)
	{
		if (st.rollno != no)
		{
			write(fd2, &st, sizeof(st));
		}
	}
	close(fd2);
	close(fd);
	remove("student.dat");
	rename("Temp.dat", "student.dat");
	printf("\n\n\tRecord Deleted ..");
	getch();
}

//***************************************************************
//        THE MAIN FUNCTION OF PROGRAM
//****************************************************************
void main()
{
	char ch;
	int num;
	do
	{
		system("clear");
		
		printf("\n\n\t1.CREATE STUDENT RECORD");
		printf("\n\n\t2.DISPLAY ALL STUDENTS RECORDS");
		printf("\n\n\t3.SEARCH STUDENT RECORD ");
		printf("\n\n\t4.MODIFY STUDENT RECORD");
		printf("\n\n\t5.DELETE STUDENT RECORD");
		printf("\n\n\t6.EXIT");
		printf("\n\n\tPlease Enter Your Choice (1-6) ");
		ch = getch();

		switch (ch)
		{
		case '1':	system("clear");
					write_student();
					break;
		case '2':	getchar();
					display_all();
					break;
		case '3':	system("clear");
					printf("\n\n\tPlease Enter The roll number ");
					scanf("%d", &num);
					display_sp(num);
					break;
		case '4':	modify_student(); break;
		case '5':	delete_student(); break;
		case '6':	break;
		default:	printf("\a");
		}
	} while (ch != '6');
}
//***************************************************************
//                END OF PROJECT
//***************************************************************
