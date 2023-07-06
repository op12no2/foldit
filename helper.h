//{{{}}}

#define __HELPER_H__

#define MEM(p) if (!(p)) MemoryProblem()
extern CFolditApp *App();
extern void NotImplementedYet();
extern void MemoryProblem();
extern BOOL OneTimeMessage (const char *, const char *);
extern void WarningBox(const char *);
extern void ImplementationRestriction(const char *);
extern void EnvironmentRestriction(const char *);
//void __far *operator new(size_t size);

